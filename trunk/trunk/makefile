#Copyright Jordan Earls <http://Earlz.biz.tm>
#All rights reserved.
#See "copyright.txt" for information on the copyright of this project and source file
#This file is part of the "Robot AI" project.

#Robots AI makefile
#only the default target does anything..
#it requires SDL libraries in your linkers lib search dir
#it also requires for the SDL shared library to be in the same directory as the produced binary(or in your PATH var)

# OS Detection..
OS := $(shell uname)

# Objects..
OBJS=obj/main.o obj/logicode.o

# Settings..
ifeq ($(OS), OpenBSD)
CPPFLAGS=-Wall -g -pthread -I./include -I/usr/local/include/SDL
LDFLAGS=-L/usr/local/lib -L/usr/X11R6/lib -lSDL -lX11 -lpthread
BINARY=Robots_AI
else
CPPFLAGS=-Wall -g -I./include
LDFLAGS=-lSDL
BINARY=Robots_AI
endif

default:
	g++ $(CPPFLAGS) -c src/main.cpp -o obj/main.o
	g++ $(CPPFLAGS) -c src/LogiCode.cpp -o obj/logicode.o

	g++ $(OBJS) $(LDFLAGS) -o $(BINARY)

clean:
	rm -f $(OBJS)
