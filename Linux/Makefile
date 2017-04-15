Files := infrared dht11 service main
SOURCE := $(foreach n,$(Files),$(n).o)
CC := gcc

REQUIREMENT:=-Wl,--start-group -lwiringPi -lpthread -ljpeg -Wl,--end-group

app:$(SOURCE)
	$(CC) $^ -o $@ $(REQUIREMENT)
infrared.o : infrared.c infrared.h sharing_data.h
	$(CC) -c infrared.c
dht11.o : dht11.c dht11.h sharing_data.h
	$(CC) -c dht11.c
service.o : service.c service.h sharing_data.h
	$(CC) -c service.c
main.o : main.c main.h sharing_data.h
	$(CC) -c main.c
rm:
	-rm -rf app *.o *.bak
