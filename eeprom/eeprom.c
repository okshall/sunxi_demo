#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "info.h"
#define EEPROM_PATH "/sys/class/i2c-dev/i2c-3/device/3-0050/eeprom"
eepromData epdata;


int crc16_check(const eepromData *epdata)
{
	unsigned short crc;
	crc = crc16(0,epdata, sizeof(eepromData) - 2);
	if (crc != epdata->crc16)
	  return -1;
	else
		return 0;
}
int crc16_update(eepromData *epdata)
{
	unsigned short crc;
	crc = crc16(0,epdata, sizeof(eepromData) - 2);
	
	epdata->crc16 = crc;
	return 0;
}

int read_eeprom_buf(eepromData *epdata, int len)
{
	FILE *fp = NULL;
	unsigned char buf[256], *tmp;
	int data_num = 2, ret;

	memset(buf, 0x0, sizeof(buf));
	fp = fopen(EEPROM_PATH, "r");
	if (!fp)
	  return -1;
	ret = fread(buf, len * data_num, 1, fp);
	fclose(fp);
	if (ret != 1)
	  return -1;

	tmp = buf;
	for (int i = 0; i < data_num; i++)
	{
		memcpy(epdata, tmp, len);
		if (crc16_check(epdata)!= 0)
		{
			tmp = buf + len;
		}
		else
		  return 0;
	}
	return -1;
}

int write_eeprom_buf(const eepromData *epdata, int len)
{
	FILE *fp = NULL;
	int ret;
	unsigned char buf[256];

	memset(buf, 0x0, sizeof(buf));
	if (0 != crc16_update(epdata))
	  return -1;

	fp = fopen(EEPROM_PATH, "w");
	if (!fp)
	  return -1;
	memcpy(buf, epdata, sizeof(eepromData));	
	memcpy(buf + sizeof(eepromData), epdata, sizeof(eepromData));	
	ret = fwrite(buf, len * 2, 1, fp);
	fclose(fp);
	if (ret != 1)
	  return -1;
	else
		return 0;
}
int write_mac0(eepromData *epdata, char *mac)
{

	sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &epdata->ethMac0[0],
				&epdata->ethMac0[1], &epdata->ethMac0[2],
				&epdata->ethMac0[3], &epdata->ethMac0[4],
				&epdata->ethMac0[5]);
	return 0;
}

int write_mac1(eepromData *epdata, char *mac)
{

	sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", &epdata->ethMac1[0], 
				&epdata->ethMac1[1], &epdata->ethMac1[2],
				&epdata->ethMac1[3], &epdata->ethMac1[4],
				&epdata->ethMac1[5]);

	return 0;
}
int write_serno(eepromData *epdata, char *serno)
{
	int len = strlen(serno);
	memset(epdata->serno, 0x0, SERNO_LEN);
	memcpy(epdata->serno, serno, len);

	return 0;
}

int write_hwver(eepromData *epdata, char *hwer)
{
	int len = strlen(hwer);
	memset(epdata->hwver, 0x0, HWVER_LEN);
	memcpy(epdata->hwver, hwer, len);

	return 0;
}
int write_swver(eepromData *epdata, char *swver)
{

	int len = strlen(swver);
	memset(epdata->swver, 0x0, SWVER_LEN);
	memcpy(epdata->swver, swver, len);

	return 0;
}
int usage(void)
{
	fprintf(stderr, "mac0 xx:xx:xx:xx:xx:xx\r\n");
	fprintf(stderr, "mac1 xx:xx:xx:xx:xx:xx\r\n");
	fprintf(stderr, "serno \"serial number\"\r\n");
	fprintf(stderr, "hwver \"hw version\"\r\n");
	fprintf(stderr, "swver \"sw version\"\r\n");
	fprintf(stderr, "info\r\n");
}
int main(int argc,char **argv)
{
	eepromData epdata;
	int ret;

	if (argc < 2)
	{
	  usage();
	  return -1;
	}

	memset(&epdata, 0x0, sizeof(eepromData));

	if (0 == strncmp(argv[1], "init", 4))
	{
		write_eeprom_buf(&epdata, sizeof(eepromData));
		return 0; 
	}
	else
		ret = read_eeprom_buf(&epdata, sizeof(eepromData));

	if (ret != 0)
	{
		fprintf(stderr, "read eeprom data error \r\n");
	  return -1;
	}
	
	for (int i = 1; i < argc; i=i+2)
	{
		if (0 == strncmp(argv[i], "mac0", 4))
		{
			write_mac0(&epdata, argv[i+1]);
		}
		else if (0 == strncmp(argv[i], "mac1", 4))
		{
			write_mac1(&epdata, argv[i+1]);
		}
		else if (0 == strncmp(argv[i], "serno", 5))
		{
			write_serno(&epdata, argv[i+1]);
		}
		else if (0 == strncmp(argv[i], "hwver", 5))
		{
			write_hwver(&epdata, argv[i+1]);
		}
		else if (0 == strncmp(argv[i], "swver", 5))
		{
			write_swver(&epdata, argv[i+1]);
		}
		else if (0 == strncmp(argv[i], "info", 4))
		{
			fprintf(stderr, "MAC0 %02X:%02X:%02X:%02X:%02X:%02X\r\n", epdata.ethMac0[0], epdata.ethMac0[1], epdata.ethMac0[2], epdata.ethMac0[3], epdata.ethMac0[4], epdata.ethMac0[5]);
			fprintf(stderr, "MAC1 %02X:%02X:%02X:%02X:%02X:%02X\r\n", epdata.ethMac1[0], epdata.ethMac1[1], epdata.ethMac1[2], epdata.ethMac1[3], epdata.ethMac1[4], epdata.ethMac1[5]);
			fprintf(stderr, "serno:%s\r\n", (char *)epdata.serno);
			fprintf(stderr, "hwver:%s\r\n", (char *)epdata.hwver);
			fprintf(stderr, "swver:%s\r\n", (char *)epdata.swver);
			return 0;
		}
		else
		{
			usage();
			return -1;
		}
	}
	write_eeprom_buf(&epdata, sizeof(eepromData));

	return 0;
}
