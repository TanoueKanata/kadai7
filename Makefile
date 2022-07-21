final: final.o bme280.o
	arm-linux-gnueabihf-gcc -pthread final.o bme280.o -o final
bme280.o: bme280.c
	arm-linux-gnueabihf-gcc -c bme280.c
final.o: final.c
	arm-linux-gnueabihf-gcc -pthread -c final.c

clean:
	rm *.o final
