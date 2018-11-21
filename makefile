.PHONY: default

default: 
	cd biblioteca/Debug; make;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug;make
	cd S-AFA/Debug; make;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug;make
	cd CPU/Debug; make;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug;make
	cd DAM/Debug; make;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug;make
	cd FM9/Debug; make;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug;make
	cd MDJ/Debug; make;export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/home/utnso/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug;make

clean:
	sudo rm -rf ~/so-commons-library
	cd biblioteca/Debug; make clean
	cd S-AFA/Debug; make clean
	cd CPU/Debug; make clean
	cd FM9/Debug; make clean
	cd DAM/Debug; make clean
	cd MDJ/Debug; make clean