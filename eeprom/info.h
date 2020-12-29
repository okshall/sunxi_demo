#ifndef __INCLUDE_INFO_H__
#define __INCLUDE_INFO_H__

#define MAC_LEN (6)
#define SERNO_LEN (32)
#define HWVER_LEN (16)
#define SWVER_LEN (24)

typedef struct
{
	/********256bytes:8/16bytes per page***************
	 * Start 0x00 --> End 0xFF
	 * 0x00:eepromData-->0x4F
	 * 0x80:eepromDataBackup-->0xCF
	 * 0xD0:--> 0xFF:Addr End
	 * ************************/
	unsigned char ethMac0[MAC_LEN];//mac addr
	unsigned char ethMac1[MAC_LEN];//mac addr
	unsigned char serno[SERNO_LEN];//hw serial No.
	unsigned char hwver[HWVER_LEN];//hw version
	unsigned char swver[SWVER_LEN];//Sw version
	unsigned short crc16; //crc16
	//Total 86bytes;
}eepromData;
#endif
