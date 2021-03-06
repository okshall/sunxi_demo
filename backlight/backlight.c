#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h> 
#include"sunxi_display2.h"
usage(char *argv)
{
	printf("%s backlight[0-250]\r\n", argv);
}
int main(int argc, char **argv)
{
	int dispfh;
	unsigned int arg[6];

	if (argc != 2)
	{
		usage(argv[0]);
		return -1;
	}

	if((dispfh = open("/dev/disp",O_RDWR)) == -1) {
		printf("open display device fail!\n");
		return -1;
	}
	
	arg[0] = 0;
	int bl = 0;
	bl = ioctl(dispfh, DISP_LCD_GET_BRIGHTNESS, (void*)arg);
	printf("Get backlight value:%d\r\n", bl);

	arg[0] = 0;
	arg[1] = atoi(argv[1]);
	ioctl(dispfh, DISP_LCD_SET_BRIGHTNESS, (void*)arg);

	close(dispfh);
	return 0;
}
