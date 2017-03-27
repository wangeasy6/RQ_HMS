SRC:=*.C
CC:=gcc
REQUIREMENT:=-Wl,--start-group -lwiringPi -lpthread -ljpeg -Wl,--end-group

app:$(SRC)
	$(CC) $(REQUIREMENT) $^ -o $@
rm:
	-rm app *.bak
