all: clean libpayload.so


clean: 
	rm -f *.o *.so 

libpayload.so: payload.o
	g++ -shared $^ -o $@ -Wl -limobiledevice-1.0 -lirecovery-1.0

payload.so: payload.o
	g++ -shared -o -limobiledevice-1.0 -lirecovery-1.0 libpayload.so payload.o

payload.o: payload.cpp 
	g++ -c -Wall -Werror -fpic payload.cpp


