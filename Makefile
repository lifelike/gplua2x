linux:
	$(MAKE) -f Makefile.linux

freebsd:
	gmake -f Makefile.freebsd

gp2x:
	$(MAKE) -f Makefile.gp2x

all: gp2x linux

dist: clean gp2x
	sh ./dist.sh

clean:
	-rm -rf *~ build *.bak
	$(MAKE) -C lua clean