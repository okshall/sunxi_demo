#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <asm/types.h>
#include <time.h>
#include <linux/fb.h>
#include "./drv_display.h"


static void  display_bootlogo(int width,int height, int status)
{
	disp_layer_config layer_para;
	int screen_width, screen_height;
	unsigned int arg[4];
	int screen_id = 1; //0 lcd, 1 hdmi
	int ret;
	int dispfh;

	if((dispfh = open("/dev/disp",O_RDWR)) == -1) {
		printf("open display device fail!\n");
		return -1;
	}
	arg[0] = screen_id;
	arg[1] = DISP_OUTPUT_TYPE_HDMI;
	arg[2] = DISP_TV_MOD_1080P_60HZ;
	ioctl(dispfh, DISP_DEVICE_SWITCH, (void *)arg);

	arg[0] = screen_id;
	screen_width = ioctl(dispfh, DISP_GET_SCN_WIDTH, (void*)arg);
	screen_height = ioctl(dispfh, DISP_GET_SCN_HEIGHT, (void*)arg);
	printf("hdmi screen_id =%d, screen_width =%d, screen_height =%d bmp_width=%d, bmp_height=%d status=%d\n", 
				screen_id, screen_width, screen_height, width, height, status);
	memset(&layer_para, 0, sizeof(disp_layer_config));
	layer_para.info.fb.addr[0] = 0x46400036; //addr get from uboot
	layer_para.channel = 1; //channel 1
	layer_para.layer_id = 0; //layer 0
	layer_para.info.fb.format = DISP_FORMAT_RGB_888;
	layer_para.info.fb.size[0].width = width;
	layer_para.info.fb.size[0].height = height;
	layer_para.info.fb.crop.x = 0;
	layer_para.info.fb.crop.y = 0;
	layer_para.info.fb.crop.width  = ((unsigned long long)width) << 32;
	layer_para.info.fb.crop.height = ((unsigned long long)height) << 32;
	layer_para.info.fb.flags = DISP_BF_NORMAL;
	layer_para.info.fb.scan = DISP_SCAN_PROGRESSIVE;
	layer_para.info.mode     = LAYER_MODE_BUFFER;
	layer_para.info.alpha_mode    = 1;
	layer_para.info.alpha_value   = 0xff;
	layer_para.info.screen_win.x   = 0;
	layer_para.info.screen_win.y   = 0;
	layer_para.info.screen_win.width       = screen_width;
	layer_para.info.screen_win.height      = screen_height;
	layer_para.info.b_trd_out              = 0;
	layer_para.info.out_trd_mode   = 0;
	layer_para.info.zorder = 0; // 0 最低优先

	layer_para.enable = status; //0-disable, 1-enable

	arg[0] = screen_id;
	arg[1] = (int)&layer_para;
	arg[2] = 1;
	ret = ioctl(dispfh, DISP_LAYER_SET_CONFIG, (void*)arg);
	if(0 != ret)
	  printf("hdmi bmp fail to set layer info\n");

	close(dispfh);

	return 0;
}
static void usage(char *name)
{
	fprintf(stderr, "   Usage: %s [-w width] [-h height] [-s status]\n", name);
	fprintf(stderr, "   -w: BMP width\n");
	fprintf(stderr, "   -h: BMP height\n");
	fprintf(stderr, "   -s: 0-disable, 1-enable layer\n");
}

int main(int argc, char *argv[])
{

	int c;
	int width = 1920, height = 1080, status = 1;

	do {
		c = getopt(argc, argv, "w:h:s:");
		if (c == EOF)
		  break;
		switch (c) {
			case 'w':
				if (optarg)
				  sscanf(optarg, "%d\n", &width);
				break;
			case 'h':
				if (optarg)
				  sscanf(optarg, "%d\n", &height);
				break;
			case 's':
				if (optarg)
				  sscanf(optarg, "%d\n", &status);
				break;
			default:
				usage(argv[0]);
				exit(1);
				break;
		}
	} while (1);

	if (optind != argc) {
		usage(argv[0]);
		exit(1);
	}
	display_bootlogo(width, height, status);
	return 0;
}

