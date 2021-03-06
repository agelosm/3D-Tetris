CC = g++

GCCFLAGS =
CFLAGS = -g
ifeq ($(OSTYPE),solaris)	
INCLUDE = -I/usr/local/include 
LDFLAGS = -L/usr/local/lib/ 
LDLIBS =  -lGL -lglut -lm 
else
INCLUDE = -I/usr/include
LDFLAGS = -L/usr/lib/
LDLIBS =  -lGLU -lglut -lm -lGL
endif


all:
	$(CC) $(GCCFLAGS) $(INCLUDE) $(CFLAGS) graph2.cpp $(LDFLAGS) $(LDLIBS) -o graph2
clean:
	rm graph2
default:
	all
