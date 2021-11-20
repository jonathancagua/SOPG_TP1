/**
 * @file reader.c
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
#include <stdio.h>

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 300

#define FILE_SIGNAL     "Sign.txt"
#define FILE_DATA       "Log.txt"

#define DATA_ENCA       "DATA:"
#define SIGN_ENCA       "SIGN:"

#define Appends_file    "a"
int main(void)
{
	uint8_t inputBuffer[BUFFER_SIZE];
	int32_t bytesRead, returnCode, fd;
    FILE *f_sign, *f_data;
    //el archivo es creado si es q no existe.
    if ( (f_sign = fopen(FILE_SIGNAL, Appends_file)) == NULL )
    {
        printf("Error abriendo archivo de SIGNAL\n");
    }
    //el archivo es creado si es q no existe.
    if ( (f_data = fopen(FILE_DATA, Appends_file)) == NULL )
    {
        printf("Error abriendo archivo de DATA\n");
    }
    /* Create named fifo. -1 means already exists so no action if already exists */
    if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1  )
    {
        printf("Error creating named fifo: %d\n", returnCode);
        exit(1);
    }
    
    /* Open named fifo. Blocks until other process opens it */
	printf("waiting for writers...\n");
	if ( (fd = open(FIFO_NAME, O_RDONLY) ) < 0 )
    {
        printf("Error opening named fifo file: %d\n", fd);
        exit(1);
    }
    
    /* open syscalls returned without error -> other process attached to named fifo */
	printf("got a writer\n");

    /* Loop until read syscall returns a value <= 0 */
	do
	{
        /* read data into local buffer */
		if ((bytesRead = read(fd, inputBuffer, BUFFER_SIZE)) == -1)
        {
			perror("read");
        }
        else
		{
			inputBuffer[bytesRead] = '\0';
			printf("reader: read %d bytes: \"%s\"\n", bytesRead, inputBuffer);

            if (strncmp((const char *)inputBuffer, SIGN_ENCA, 5) == 0) {
                fprintf(f_sign, "%s \n", inputBuffer);
                fflush(f_sign);
            }

            if (strncmp((const char *)inputBuffer, DATA_ENCA, 5) == 0) {
                fprintf(f_data, "%s \n", inputBuffer);
                fflush(f_data);
            }
		}
	}
	while (bytesRead > 0);

	return 0;
}
