

CROSS_COMPILE ?= arm-linux-gnueabihf-

#RAMDISK_DIR = ../ramdisk/usr/sbin

#CFLAGS = -Wall

#ifeq (none-linux-gnueabi, $(findstring none-linux-gnueabi, $(CROSS_COMPILE)))
#	CFLAGS += -march=armv4t
#	LDFLAGS += -march=armv4t
#endif

EXE=smi-tool

all: $(EXE)

smi-tool: smi-tool.c
	$(CROSS_COMPILE)gcc -o smi-tool $(CFLAGS) smi-tool.c

clean:
	$(RM) $(EXE)

install: $(EXE)
	#@install -D -m 755 $(EXE) $(RAMDISK_DIR)/
	cp -r $(EXE) $(DESTDIR)

