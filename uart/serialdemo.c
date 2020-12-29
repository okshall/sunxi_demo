
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "./serial_sdk.h"
#define TIMEOUT 2000000

void static usage(char *name)
{
	printf("%s ",name);
	printf("-d serialname -b baudrate\n");
}

int main(int argc, char **argv)
{
	int fd0;
	char dev0[64]="/dev/ttyS0";
	char wbuf[512]={0x00};
	char rbuf[512]={0x00};
	int loop =10;
	int readcount=0, ret, i;
	int c;
	if(argc<2) {
		usage(argv[0]);
		exit(0);
	}

	struct ttyattr attr;
	attr.flow_control = 0;
	attr.baudrate  = 9600;
	attr.databits  = 8;
	attr.parity    = 0;
	attr.stopbits  = 1;


	while ((c = getopt(argc, argv, "d:b:")) != EOF) {
		switch(c){
			case 'd':
				sprintf(dev0,"%s\0",optarg);
				break;
			case 'b':
				attr.baudrate=atoi(optarg);
				break;
			default:
				usage(argv[0]);
				exit(0);
		}
	}
	fd0 = open_serial(dev0,&attr);
	if(fd0<0) {
		printf("open %s is error\n",dev0);
		return 0;
	}
	flush_serial_r(fd0);
	while(loop--){
		memset(rbuf,0x0, sizeof(rbuf));
		memset(wbuf,0x0, sizeof(wbuf));
		readcount=read_serial(fd0,rbuf,sizeof(rbuf), TIMEOUT);
		if (readcount > 0){
			sprintf(wbuf,"Read data[%s]", rbuf);
			write(fd0,wbuf,strlen(wbuf));	
			printf("write: %s\n",wbuf);
		}
	}
	close(fd0);
}




