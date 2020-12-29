/*
mdio-tool allow for direct access to mdio registers in a network phy.

Routines are based on mii-tool: http://freecode.com/projects/mii-tool

mdio-tool comes with ABSOLUTELY NO WARRANTY; Use with care!

Copyright (C) 2013 Pieter Voorthuijsen

mdio-tool is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

mdio-tool is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with mdio-tool.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>

#ifndef __GLIBC__
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#endif
#include "mii.h"

#define SIOCGSMIREG    0x894C          /* Read SMI PHY register.       */
#define SIOCSSMIREG    0x894D          /* Write SMI PHY register.      */

#define MAX_ETH		8		/* Maximum # of interfaces */

static int skfd = -1;		/* AF_INET socket for ioctl() calls. */
static struct ifreq ifr;


char inittab[][2] =
{
	{0x14, 0x01}, // Port 1 Control 4, Default port’s tag VID
	{0x24, 0x02}, // Port 2 Control 4, Default port’s tag VID
	{0x05, 0x80}, // 802.1q vlan enable
	{0x81, 0x0D},
	{0x82, 0x10},
	{0x83, 0x01},
	{0x79, 0x04}, // 0100 = VLAN table selected
	{0x7A, 0x06},
	{0x81, 0x0E},
	{0x82, 0x20},
	{0x83, 0x02},
	{0x79, 0x04},
	{0x7A, 0x06},
	
	{0x30, 0x04},
	{0x10, 0x02},
	{0x20, 0x02},
	{0x01, 0x01}, // start switch bit set 1
};




/*--------------------------------------------------------------------*/

static int mdio_read(int skfd, int location, int port)
{
    struct mii_data *mii = (struct mii_data *)&ifr.ifr_data;
    mii->reg_num = location;
    mii->phy_id  = port;

    if (ioctl(skfd, SIOCGSMIREG, &ifr) < 0) {
	fprintf(stderr, "SIOCGSMIREG on %s failed: %s\n", ifr.ifr_name,
		strerror(errno));
	return -1;
    }
    return mii->val_out;
}

static void mdio_write(int skfd, int location, int port, int value)
{
    struct mii_data *mii = (struct mii_data *)&ifr.ifr_data;
    mii->reg_num = location;
    mii->phy_id  = port;
    mii->val_in = value;

    if (ioctl(skfd, SIOCSSMIREG, &ifr) < 0) {
	fprintf(stderr, "SIOCSSMIREG on %s failed: %s\n", ifr.ifr_name,
		strerror(errno));
    }
}


int main(int argc, char **argv)
{
	int addr, dev, val, port;
	struct mii_data *mii = (struct mii_data *)&ifr.ifr_data;

	if(argc < 2) {
		printf("Usage smi-tool [r/w] [dev] [reg] [val]\n");
		return 0;
	}
	/* Open a basic socket. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
		perror("socket");
		return -1;
	}

	/* Get the vitals from the interface. */
	strncpy(ifr.ifr_name, argv[2], IFNAMSIZ);
	if (ioctl(skfd, SIOCGMIIPHY, &ifr) < 0) {
		if (errno != ENODEV)
		fprintf(stderr, "SIOCGMIIPHY on '%s' failed: %s\n",
			argv[2], strerror(errno));
		return -1;
	}

	if(argv[1][0] == 'r') {
		addr = strtol(argv[3], NULL, 16);
		printf("0x%.4x\n", mdio_read(skfd, addr, 0));
	}
	else if(argv[1][0] == 'w') {
		addr = strtol(argv[3], NULL, 16);
		val = strtol(argv[4], NULL, 16);
		mdio_write(skfd, addr, 0, val);
	}
	else {
		printf("Fout!\n");
	}

	close(skfd);
}
