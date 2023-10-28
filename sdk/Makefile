
ARCH ?= x86_64
IS_32BIT ?= n

ifeq ($(ARCH),x86_64)
   TARGETS =
else
   TARGETS = 
#    TARGETS = kern
endif

all: autoconfig
	xmake -v $(TARGETS)

autoconfig:
	xmake f -a $(ARCH) --32bit=$(IS_32BIT)

config:
	xmake f --menu

clean:
	xmake c

dist-clean:
	rm -rf build .xmake

