#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WRKDIR = `pwd`
MAKE = make

all: bullettest_bullettest_linux

bullettest_bullettest_linux: 
	$(MAKE) -C BulletTest all

clean: clean_bullettest_bullettest_linux

clean_bullettest_bullettest_linux: 
	$(MAKE) -C BulletTest clean

.PHONY: clean_bullettest_bullettest_linux

bullet:
	(test -d ./tmp || mkdir ./tmp)
	cd ./tmp &&\
	wget -O bullet-2.82-r2704.tgz -N "https://bullet.googlecode.com/files/bullet-2.82-r2704.tgz" &&\
	tar zxvf bullet-2.82-r2704.tgz
	cd ./tmp/bullet-2.82-r2704 &&\
	(test -d Unix\ Makefiles || mkdir Unix\ Makefiles) &&\
	cd Unix\ Makefiles &&\
	cmake -G "Unix Makefiles" ../ &&\
	make BulletCollision BulletDynamics LinearMath &&\
	cd ./src &&\
	rsync -arv --include '*/' --include '*.h' --exclude '*' . ../../../../include &&\
	find . -name '*.a' -exec cp {} ../../../../lib/linux \;
	
.PHONY: bullet