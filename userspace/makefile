ARCH ?= x86_64
IS_32BIT ?= n

all: autoconfig
	xmake -v

autoconfig:
	xmake f -a $(ARCH) --32bit=$(IS_32BIT)
	
config:
	xmake f --menu

clean:
	xmake c

dist-clean:
	rm -rf build .xmake