
all: bencode_pretty bencode_prettier bencode_prettiest bencode_unpretty

bencode_pretty.o: bencode_pretty.c bencode_scan.c

bencode_prettier.o: bencode_prettier.c bencode_scan.c

bencode_prettiest.o: bencode_prettiest.c bencode_scan.c

bencode_unpretty.o: bencode_unpretty.c bencode_scan.c

bencode_pretty: bencode_pretty.o

bencode_prettier: bencode_prettier.o

bencode_prettiest: bencode_prettiest.o

bencode_unpretty: bencode_unpretty.o

clean:
	rm -f *.o core

.PHONY: all clean
