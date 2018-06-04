# This makefile build the source and install the libraries

SYSTEM = unix

ifeq ($(SYSTEM), unix)
     include ./unix.cfg
endif

ifeq ($(SYSTEM), mingw)
     include ./mingw.cfg
endif

ifeq ($(SYSTEM), mac)
     include ./mac.cfg
endif


# Libraries needed: SDL, SDL-Image, SDL-ttf, GLEW, OpenGL
LIBS = $(B_LIBS)
CFLAGS = $(B_CFLAGS) -I"$(BASEINCPATH)" -I"$(UIINCPATH)" -I"$(VOLUMINCPATH)" \
         $(FLAGS)

DESTOBJ = ./bin/
DESTLIB = ./lib/
BASESRC = menu.o stipple.o random.o

.PHONY: clean mrproper uninstall install documentation


# Add here all the test programs:
all: $(DESTOBJ)stipp$(EXEC) 


# Make the program
$(DESTOBJ)stipp$(EXEC): stipp.o $(BASESRC)\
                        $(BASELIBPATH)libPOSbase.a \
                        $(UILIBPATH)libPOSui.a \
                        $(VOLUMLIBPATH)libPOSVolum.a 

	$(LD) $(CFLAGS) $(BASESRC) stipp.o \
                        -L"$(UILIBPATH)" -lPOSui \
                        -L"$(VOLUMLIBPATH)" -lPOSVolum \
                        -L"$(BASELIBPATH)" -lPOSbase \
                        -o $(DESTOBJ)stipp$(EXEC) $(LIBS) 

stipple.o: stipple.cc
	$(CC) $(CFLAGS) -c stipple.cc

stipp.o: stipp.cc
	$(CC) $(CFLAGS) -c stipp.cc

random.o: random.cc
	$(CC) $(CFLAGS) -c random.cc

menu.o: menu.cc
	$(CC) $(CFLAGS) -c menu.cc


# Cleaning functions
# mrproper option should be passed every time you commit the code
clean:
	$(RM) *~ *.o

mrproper:
	$(RM) *~ *.o $(DESTOBJ)stipp$(EXEC)

