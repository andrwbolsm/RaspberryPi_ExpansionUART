#include <wiringSerial.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>     //exit()
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>

#define UART_DEV2    "/dev/ttySC1"
#define BAUDRATE 57600

int fd;

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:serialClose \r\n");
    serialClose(fd);

    exit(0);
}

int main(void)
{
    if((fd = serialOpen (UART_DEV2, BAUDRATE)) < 0) {
        printf("serial err\n");
        return -1;
    }
    printf("RADIO RX\r\n");

    char str;
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    while(1) 
    {
        str = serialGetchar (fd);
        
        printf("%c", str);
        fflush (stdout) ;
    }

    return 0;
}
