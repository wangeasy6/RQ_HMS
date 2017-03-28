SOURCE:=*.c
CC:=gcc
REQUIREMENT:=-Wl,--start-group -lwiringPi -lpthread -ljpeg -Wl,--end-group

app:$(SOURCE)
	$(CC) $(REQUIREMENT) $^ -o $@
rm:
	-rm -rf app *.bak
