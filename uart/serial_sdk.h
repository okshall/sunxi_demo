#ifndef SERIAL_SDK_H 
#define SERIAL_SDK_H

#include <stdio.h>
#include <termios.h>


struct ttyattr {
	int flow_control; /* 0 -> off, 1 -> on */
	int databits; /* 5 6 7 8 */
	int stopbits; /* 1 or 2 */
	int baudrate; /* 38400, or 9600 , or others. */
	int parity;  /* 0-> no, 1->odd, 2->even */
};


/* 
 * Open Serial
 * Input
 *       serial_dev  : serial device name with absolute path
 *       attr        : serial configure parameters
 *
 * Return
 *       -1          : Error
 *        > 0        : The file descriptor
 */
int open_serial(char *serial_dev, struct ttyattr *attr);


/* 
 * Close Serial
 * Input
 *        fd         : The file descriptor
 * It always return 0
 */
int close_serial(int fd);


/* 
 * Read out all buffered data and discard it
 * Input
 *       fd          : The file descriptor
 * Return
 *       -1          : Error
 *        0          : Ok
 */
int flush_serial_r(int fd);


/* 
 * Read 'bufsize' of bytes from the descriptor 'fd' to 'buf'
 * Input
 *      fd           : The file descriptor
 *      buf          : The target buffer pointer
 *      bufsize      : The number of bytes required to read
 *      timeout_us   : Timeout value in micro-seconds
 * Return
 *      -1           : Error
 *       0           : Timeout and read nothing
 *       >0          : Numbers of bytes read
 */
int read_serial(int fd, unsigned char *buf, int bufsize, long timeout_us);


/* 
 * Write 'bufsize' of bytes from the 'buf' to the descriptor 'fd'
 * Input
 *      fd           : The file descriptor
 *      buf          : The source buffer pointer
 *      bufsize      : The number of bytes required to write
 *      timeout_us   : Timeout value in micro-seconds
 * Return
 *      -1           : Error
 *       0           : Timeout and write nothing
 *       >0          : Numbers of bytes write
 */
int write_serial(int fd, unsigned char *buf, int bufsize, long timeout_us);


#endif

