CC            = arm-linux-gnueabihf-gcc
CXX           = arm-linux-gnueabihf-g++
STRIP		  = arm-linux-gnueabihf-strip
CFLAGS        = -O2
CXXFLAGS      = -O2


OBJS=hdmi_bootlogo.o 

NAME=hdmi_bootlogo

all: $(NAME)
	
$(NAME): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@
	$(STRIP) $(NAME)

clean:
	rm -rf $(OBJS) $(NAME) > /dev/null 2>&1

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
