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
#include "serial_sdk.h"
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>


static void __set_baudrate(struct termios *io, struct ttyattr *attr)
{	
    speed_t speed = B9600;
    switch (attr->baudrate) {
	case 9600:
	    speed = B9600;
	    break;
	case 19200:
	    speed = B19200;
	    break;
	case 38400:
	    speed = B38400;
	    break;
	case 115200:
	    speed = B115200;
	    break;
	case 4800:
	    speed = B4800;
	    break;
	case 2400:
	    speed = B2400;
	    break;
	case 1200:
	    speed = B1200;
	    break;
	case 600:
	    speed = B600;
	    break;
	case 300:
	    speed = B300;
	    break;
	case 57600:
	    speed = B57600;
	    break;
	case 230400:
	    speed = B230400;
	    break;
	case 460800:
	    speed = B460800;
	    break;
	case 921600:
	    speed = B921600;
	    break;
	case 1000000:
	    speed = B1000000;
	    break;
	case 1152000:
	    speed = B1152000;
	    break;
	case 2000000:
	    speed = B2000000;
	    break;
	case 4000000:
	    speed = B4000000;
	    break;
    }
    cfsetispeed(io, speed);
    cfsetospeed(io, speed);
}
#define CRTSCTS 020000000000
static void __set_fc(struct termios *io, struct ttyattr *attr)
{
#if 0
    if (attr->flow_control > 0 ) {
	io->c_iflag |= IXON;
	io->c_iflag &= ~IXOFF;
    } else if (attr->flow_control < 0 ) {
	io->c_iflag &= ~IXON; /*disable*/
	io->c_iflag &= ~IXOFF;
    } else {
	io->c_iflag &= ~IXON;
	io->c_iflag |= IXOFF;
    }
#endif
    if (attr->flow_control == 1){
	io->c_cflag &= ~CRTSCTS;
	io->c_iflag |= (IXON | IXOFF | IXANY);
    }else if( attr->flow_control == 2){
	io->c_iflag &= ~(IXON | IXOFF | IXANY);
	io->c_cflag |= CRTSCTS;
    }else{
	io->c_cflag &= ~CRTSCTS;
	io->c_iflag &= ~(IXON | IXOFF | IXANY);
    }
}


static void __set_databits(struct termios *io, struct ttyattr *attr)
{
    switch (attr->databits) {
	case 5:
	    io->c_cflag |= CS5;
	    break;
	case 6:
	    io->c_cflag |= CS6;
	    break;
	case 7:
	    io->c_cflag |= CS7;
	    break;
	case 8:
	    io->c_cflag |= CS8;
	    break;
    }
}

static void __set_parity(struct termios *io, struct ttyattr *attr)
{
    switch (attr->parity) {
	case 0:
	    io->c_cflag &= ~PARENB;  /* Clear parity enable */
	    io->c_iflag &= ~INPCK;   /* disable parity checking */
	    break;
	case 1:
	    io->c_cflag |= (PARODD | PARENB); 
	    io->c_iflag |= INPCK;    /* odd parity checking */
	    break;
	case 2:
	    io->c_cflag |= PARENB;   /* Enable parity */
	    io->c_cflag &= ~PARODD;
	    io->c_iflag |= INPCK;    /*enable parity checking */
	    break;
    }
}

static void __set_stopbits(struct termios *io, struct ttyattr *attr)
{
    switch (attr->stopbits) {
	case 1:
	    io->c_cflag &= ~CSTOPB;
	    break;
	case 2:
	    io->c_cflag |= CSTOPB;
	    break;
    }
}

static void __set_lf(struct termios *io)
{
    io->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    io->c_oflag &= ~OPOST;
}

int open_serial(char *serial_dev, struct ttyattr *attr)
{
    int fd;
    struct termios tio;
    fd = open(serial_dev, O_RDWR|O_NONBLOCK);
    /*fd = open(serial_dev, O_RDWR);*/
    if (fd < 0)
	return -1;

    tcgetattr(fd, &tio);
    cfmakeraw(&tio);
    __set_fc(&tio, attr);
    __set_baudrate(&tio, attr);
    __set_databits(&tio, attr);
    __set_parity(&tio, attr);
    __set_stopbits(&tio, attr);
    __set_lf(&tio);
    tcsetattr(fd, TCSANOW, &tio);
    return fd;
}

/* Flush serial descriptor reading buffer */
int flush_serial_r(int fd)
{
    int flag=0, isblock=0;
    char buf[64];

    if (fd < 0)
	return -1;

    /* set it to nonblock */
    if (fcntl(fd, F_GETFL, &flag) < 0)
	return -1;

    if (flag & O_NONBLOCK)
	isblock = 0;
    else
	isblock = 1;

    if (isblock) {
	flag |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flag) < 0)
	    return -1;
    }

    /* read out */
    while (read(fd, buf, sizeof(buf)) > 0);

    /* restore it to block */
    if (isblock) {
	flag &= ~O_NONBLOCK;
	fcntl(fd, F_SETFL, flag);
    }

    return 0;
}

int read_serial(int fd, unsigned char *buf, int bufsize, long timeout_us)
{
    int ret=-1, err=0, nr=0;
    fd_set rfds, efds;
    struct timeval to;

    if (fd < 0)
	return -1;

    if (!buf)
	return -1;

    if (bufsize < 0)
	return -1;

    if (timeout_us < 0)
	return -1;

    while (bufsize) {

	FD_ZERO(&rfds);
	FD_ZERO(&efds);
	FD_SET(fd, &rfds);
	FD_SET(fd, &efds);

	to.tv_sec  = timeout_us / 1000000;
	to.tv_usec = timeout_us % 1000000;

	ret = select(fd+1, &rfds, NULL, &efds, &to);

	if (ret < 0) {
	    ++err;
	    break;
	}

	else if (ret == 0) {

	    /* timeout */
	    break;
	}

	else {

	    if (FD_ISSET(fd, &rfds)) {

		ret = read(fd, buf, bufsize);
		if (ret < 0) {
		    ++err;
		    break;
		}

		buf += ret;
		bufsize -= ret;
		nr += ret;
	    }

	    if (FD_ISSET(fd, &efds)) {
		++err;
		break;
	    }

	    /* time left */
	    timeout_us = to.tv_sec*1000000 + to.tv_usec;
	}
    } /* while (bufsize) */


    if (nr > 0)
	return nr;

    if (err)
	return -1;

    return 0;
}

int write_serial(int fd, unsigned char *buf, int bufsize, long timeout_us)
{
    int ret=-1, err=0, nr=0;
    fd_set wfds, efds;
    struct timeval to;

    if (fd < 0)
	return -1;

    if (!buf)
	return -1;

    if (bufsize < 0)
	return -1;

    if (timeout_us < 0)
	return -1;

    while (bufsize) {

	FD_ZERO(&wfds);
	FD_ZERO(&efds);
	FD_SET(fd, &wfds);
	FD_SET(fd, &efds);

	to.tv_sec  = timeout_us / 1000000;
	to.tv_usec = timeout_us % 1000000;

	ret = select(fd+1, NULL, &wfds, &efds, &to);

	if (ret < 0) {
	    ++err;
	    break;
	}

	else if (ret == 0) {

	    /* timeout */
	    break;
	}

	else {

	    if (FD_ISSET(fd, &wfds)) {

		ret = write(fd, buf, bufsize);
		if (ret < 0) {
		    ++err;
		    break;
		}

		buf += ret;
		bufsize -= ret;
		nr += ret;
	    }

	    if (FD_ISSET(fd, &efds)) {
		++err;
		break;
	    }

	    /* time left */
	    timeout_us = to.tv_sec*1000000 + to.tv_usec;
	}
    } /* while (bufsize) */


    if (nr > 0)
	return nr;

    if (err)
	return -1;

    return 0;
}

int close_serial(int fd)
{
    if (fd > 0)
	close(fd);

    return 0;
}

