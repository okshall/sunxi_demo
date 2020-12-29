CC            = ../../tools/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CXX           = ../../tools/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
STRIP		  = ../../tools/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-strip
CFLAGS        = -O2
CXXFLAGS      = -O2

OBJS=serial_sdk.o serialdemo.o \

NAME=serialdemo

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@
	$(STRIP) $(NAME)
clean:
	rm -rf $(OBJS) $(NAME) > /dev/null 2>&1
install:
	cp $(NAME) ../../pack/chips/sun8iw11p1/boot-resource/public_dav/bin/
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

