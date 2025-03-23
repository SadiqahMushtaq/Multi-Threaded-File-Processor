CC=gcc
CFLAGS=-Wall -Wextra -Wno-long-long -Wno-overflow

build_st:
	$(CC) $(CFLAGS) -o ST ST.c
build_mt:
	$(CC) $(CFLAGS) -o MT MT.c

run_st:
	./ST Sadiqah data_tiny.txt 
	./ST Sadiqah data_small.txt
	./ST Sadiqah data_medium.txt
	./ST Sadiqah data_large.txt
run_mt:
	./MT Sadiqah data_tiny.txt 4
	./MT Sadiqah data_tiny.txt 10
	./MT Sadiqah data_tiny.txt 25
	./MT Sadiqah data_tiny.txt 50
	./MT Sadiqah data_tiny.txt 100

clean:
	rm -f ST
	rm -f MT
