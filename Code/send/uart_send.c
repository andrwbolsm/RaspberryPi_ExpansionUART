#include <wiringSerial.h>
#include <wiringPi.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>     //exit()
#include <signal.h>

#define UART_DEV1 "/dev/ttySC0"
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
    
    if ((fd = serialOpen(UART_DEV1, BAUDRATE)) < 0)
    {
        printf("serial err\n");
        return -1;
    }
    printf("RADIO TX\r\n");

    char c;
    
    signal(SIGINT, Handler);
    
    while(1)
    {
        c = getchar();
        
        serialPutchar(fd, c);
    }

    return 0;
}
