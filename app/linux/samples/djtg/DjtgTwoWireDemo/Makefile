# File: makefile
# Author: MTA
# Company: Digilent Inc.
# Date: 07/17/2012
# Description: makefile for Adept SDK DjtgTwoWireDemo

CC = gcc
INC = /usr/local/include/digilent/adept
LIBDIR = /usr/local/lib/digilent/adept
TARGETS = DjtgTwoWireDemo
CFLAGS = -I $(INC) -L $(LIBDIR) -ldjtg -ldmgr

all: $(TARGETS)

DjtgTwoWireDemo:
	$(CC) -o DjtgTwoWireDemo DjtgTwoWireDemo.cpp $(CFLAGS)
	

.PHONY: vclean

vclean:
	rm -f $(TARGETS)

