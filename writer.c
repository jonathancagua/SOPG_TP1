/**
 * @file writer.c
 * @author jonathan.cagua@gmail.com
 * @brief Este proceso leerá los datos 
 * del named fifo y según el encabezado “DATA” o “SIGN” 
 * escribirá en el archivo log.txt o signals.txt.
 * 
 * usar para listar el PID el siguiente comando
 *      ps
 * usar para mandar signal: kill -SIGUSR1 PID
 *      kill -SIGUSR1 32615
 *      kill -SIGUSR2 32615
 * @version 0.1
 * @date 2021-11-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 300
#define SIGN_U1_MASK      "SIGN:1"
#define SIGN_U2_MASK      "SIGN:2"
int32_t fd;
void signal_handler(int sig)
{
    uint32_t len_sig;
    if (sig == SIGUSR1)
    {
        len_sig = sizeof(SIGN_U1_MASK);
        if(write(fd, SIGN_U1_MASK, len_sig) == -1)
        {
            perror("write SIGN_U1");
        }
        else
        {
            printf("writer SIGN_U1: wrote %d bytes\n", len_sig);
        }
    }

    if (sig == SIGUSR2)
    {
        len_sig = sizeof(SIGN_U2_MASK);
        if(write(fd, SIGN_U2_MASK, len_sig) == -1)
        {
            perror("write SIGN_U2");
        }
        else
        {
            printf("writer SIGN_U2: wrote %d bytes\n", len_sig);
        }
    }
    
}
int main(void)
{
   
    char outputBuffer[BUFFER_SIZE];
	uint32_t bytesWrote;
	int32_t returnCode;
    struct sigaction sa;

    /* Create named fifo. -1 means already exists so no action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1 )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }

    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for readers...\n");
	if ( (fd = open(FIFO_NAME, O_WRONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }

	sa.sa_handler = signal_handler;
	sa.sa_flags = 0; //SA_RESTART;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGUSR1,&sa,NULL);
    sigaction(SIGUSR2,&sa,NULL);
    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a reader--type some stuff\n");

    /* Loop forever */
	while (1)
	{
        /* Get some text from console */
		fgets(outputBuffer, BUFFER_SIZE, stdin);
        
        /* Write buffer to named fifo. Strlen - 1 to avoid sending \n char */
		if ((bytesWrote = write(fd, outputBuffer, strlen(outputBuffer)-1)) == -1)
        {
			perror("write");
        }
        else
        {
			printf("writer: wrote %d bytes\n", bytesWrote);
        }
	}
	return 0;
}
