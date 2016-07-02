
.PHONY: clean archc armv7 dumboot dummyos

all: archc armv7 mksd
	
archc: archc/configure archc/Makefile
	cd archc && make && make install 

archc/configure:
	cd archc && ./boot.sh 

archc/Makefile:
	cd archc && 									\
	./configure --prefix=${ARCHC_INSTALL_PATH}     	\
        --with-systemc=${SYSTEMC_INSTALL_PATH} 	   	\
        --with-tlm=${SYSTEMC_INSTALL_PATH} ;


armv7: armv7/configure armv7/Makefile 
	cd armv7 && make && make install 

armv7/configure:
	cd armv7 && ./bootstrap.sh

armv7/Makefile:
	cd armv7 && \
	./configure --prefix=${ARM_SIMULATOR_PATH} 

dumboot:
	cd dumboot && make

dummyos: 
	cd dummyos && make 

mksd: dumboot dummyos
	mksd.sh --os dumboot/dumboot.elf && \
	mv disk.img dumboot.img && \
	mksd.sh --os dummyos/knrl && \
	mv disk.img card.img

run:
	armsim --rom=dumboot.img --sd=card.img

clean: clean-archc clean-armv7 clean-dumboot clean-dummyos clean-imgs

clean-archc:
	cd archc && make clean && make distclean 

clean-armv7:
	cd armv7 && make clean && make distclean 

clean-dumboot:
	cd dumboot && make clean 

clean-dummyos:
	cd dummyos && make clean 

clean-imgs:
	rm -f *.img


