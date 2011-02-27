default: all

all: tc

tc: 
	cd src; make

clean:
	cd src; make clean

cleanall:
	rm -f tc
	cd src; make cleanall 
