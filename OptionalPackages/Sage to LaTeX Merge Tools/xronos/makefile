#
# Makefile for the xronos tools stuff
#

# various interesting targets
#
# make - makes the toolset
# make default - makes the toolset - same as make
# make help - displays help about various interesting targets


# define the compiler and tools we will use
CC = gcc
AR = ar
RM = rm

# little to no optimization be tough on warnings
CFLAGS = -c -Og -g -pedantic 

LDFLAGS = 

# these headers are used by many files used to build xronosfmt
XRONOSHEADERS = xronosfmt.h xronostoollib.h danalib.h xronosio.h

# objects for my generic library, used in the tools
DANALIBOBJS = danalib.o ParseCmd.o ParseIni.o KeyValue.o

# some values used by backup target
.PHONY: backup NewConf

MYTIMESTAMP = `date +"%g%m%d%H%M%S" `
BACKUPNAME = ../xronoschkpt-$(MYTIMESTAMP)

# default target is to build everything - must be first target
default: xronosfmt xronosraw xronosbatch xronosfinal xronosmerge danalib.lib xronostoollib.lib

# Help target gives healp about this make file
help:
	@echo make - makes the toolset (same as make default)
	@echo make default - makes the toolset
	@echo make backup - backs up the currrent directory to a ../xronoschkpt-YYMMDDHHMMSS directory 
	@echo make clean - deletes the executables and the objects so the next make is a full rebuild
	@echo make install - copies the executables to /usr/local/bin to install them

# this target backs up the current directory to a sibling
# the sibling name is based on a date/time and a prefix of chkpt-
# there is some magic to ensure all lines run in the same shell
# and some additional magic to ensure shell variables are not 
# eaten by make
#
# so basically "make backup" copies everything in the current tree 
# before you unpack/edit/pull/fetch/... the next version
backup:
	@BKNAME=$(BACKUPNAME) ; \
	mkdir $$BKNAME ; \
	cp -R * $$BKNAME ; \
	echo copied files to $$BKNAME 

# the main/other targets start here
xronosraw: xronosraw.o xronostoollib.lib
	$(CC) $(LDFLAGS) -o $@ $^

xronosbatch: xronosbatch.o xronostoollib.lib
	$(CC) $(LDFLAGS) -o $@ $^

xronosfinal: xronosfinal.o BuildHeader.o xronostoollib.lib
	$(CC) $(LDFLAGS) -o $@ $^

xronosfmt: xronosfmt.o dupcontent.o braces.o QuestionLimit.o texttt.o xronostoollib.lib
	$(CC) $(LDFLAGS) -o $@ $^
	
xronosmerge: xronosmerge.o merge.o xronostoollib.lib
	$(CC) $(LDFLAGS) -o $@ $^
	
clean:
	-rm -f *.o
	-rm -f *.lib
	-rm -f xronosfmt	
	-rm -f xronosfinal
	-rm -f xronosraw 
	-rm -f xronosmerge
	-rm -f xronosbatch

# install the stuff, might need sudo to make this work
install:
	cp xronosfmt /usr/local/bin
	cp xronosfinal /usr/local/bin
	cp xronosraw /usr/local/bin
	cp xronosmerge /usr/local/bin
	cp xronosbatch /usr/local/bin
	cp ProblemConverter.sh /usr/local/bin
	chmod a+rx /usr/local/bin/xronos*
	chmod a+rx /usr/local/bin/ProblemConverter.sh
	if [ ! -e /usr/local/etc/xronostools ] ; then mkdir /usr/local/etc/xronostools ; chmod a+rx /usr/local/etc/xronostools ; fi
	if [ ! -e /usr/local/etc/xronostools/xronostools.conf ] ; then cp xronostools.conf /usr/local/etc/xronostools/xronostools.conf ; chmod a+r /usr/local/etc/xronostools/xronostools.conf ; fi


# force install the xronostools.conf overwriting any existing one
NewConf:
	if [ ! -d /usr/local/etc/xronostools ] ; then mkdir /usr/local/etc/xronostools ; chmod a+rx /usr/local/etc/xronostools ; fi
	cp xronostools.conf /usr/local/etc/xronostools/xronostools.conf ; chmod a+r /usr/local/etc/xronostools/xronostools.conf

# build rule for danalib.lib
danalib.lib: $(DANALIBOBJS)
	$(AR) rcs $@ $^
	
# build rule for xronostoollib.lib we add the danalib objects to have one lib for link
xronostoollib.lib: xronostoollib.o xronosio.o xtl_subvars.o $(DANALIBOBJS)
	$(AR) rcs $@ $^


	
# various object file builds and dependancies
xronosbatch.o: xronosbatch.c danalib.h xronostoollib.h 
	$(CC) $(CFLAGS) $< -o $@

xronosraw.o: xronosraw.c danalib.h xronostoollib.h xronosio.h
	$(CC) $(CFLAGS) $< -o $@

xronosio.o: xronosio.c xronosio.h xronostoollib.h danalib.h
	$(CC) $(CFLAGS) $< -o $@
	
xronostoollib.o: xronostoollib.c xronostoollib.h danalib.h
	$(CC) $(CFLAGS) $< -o $@
	
ParseCmd.o: ParseCmd.c danalib.h
	$(CC) $(CFLAGS) $< -o $@
	
danalib.o: danalib.c danalib.h
	$(CC) $(CFLAGS) $< -o $@
	
# generic object file build
# we use the XRONOSHEADERS list as "better to build than be sorry" logic
%.o: %.c $(XRONOSHEADERS) 
	$(CC) $(CFLAGS) $< -o $@

