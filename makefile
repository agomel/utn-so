.PHONY: default

default: so-commons-library biblioteca S-AFA DAM FM9 MDJ CPU;

so-commons-library:
	cd ~; git clone https://github.com/sisoputnfrba/so-commons-library; cd so-commons-library; make; sudo make install

biblioteca:
	cd biblioteca/Debug; make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug

S-AFA:
	cd S-AFA/Debug; make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug

CPU:
	cd CPU/Debug; make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug

DAM:
	cd DAM/Debug; make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug

FM9:
	cd FM9/Debug; make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug

MDJ:
	cd MDJ/Debug; make; export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/workspace/tp-2018-2c-5-fant-sticos/biblioteca/Debug

clean:
	sudo rm -rf ~/so-commons-library
	cd biblioteca/Debug; make clean
	cd S-AFA/Debug; make clean
	cd CPU/Debug; make clean
	cd FM9/Debug; make clean
	cd DAM/Debug; make clean
	cd MDJ/Debug; make clean