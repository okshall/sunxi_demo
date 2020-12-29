#!/bin/sh
vconfig add eth0 1
vconfig add eth0 2
ifconfig eth0 0.0.0.0

smi-tool w eth0 0x01 0x00 #start switch bit set 1
smi-tool w eth0 0x05 0x80 #802.1q vlan enable

smi-tool w eth0 0x14 0x01 #Port 1 Control 4, Default port's tag VID 1
smi-tool w eth0 0x24 0x02 #Port 2 Control 4, Default port's tag VID 2


smi-tool w eth0 0x81 0x0D
smi-tool w eth0 0x82 0x10
smi-tool w eth0 0x83 0x01
smi-tool w eth0 0x79 0x04 #0100 = VLAN table selected
smi-tool w eth0 0x7A 0x00


smi-tool w eth0 0x81 0x0E
smi-tool w eth0 0x82 0x20
smi-tool w eth0 0x83 0x02
smi-tool w eth0 0x79 0x04 #0100 = VLAN table selected
smi-tool w eth0 0x7A 0x01


smi-tool w eth0 0x11 0x1F
smi-tool w eth0 0x21 0x1F
smi-tool w eth0 0x31 0x1F

smi-tool w eth0 0x30 0x04

smi-tool w eth0 0x10 0x02
smi-tool w eth0 0x20 0x02
smi-tool w eth0 0x01 0x01 #start switch bit set 1


ifconfig eth0.1 hw ether 9c:5c:8e:95:a5:32
ifconfig eth0.2 hw ether 9c:55:8e:95:b5:33
