CC = gcc

cflash: cflash.c
	${CC} cflash.c -o cflash

install: cflash
	cp cflash /usr/local/bin/cflash
	chown 0:0 /usr/local/bin/cflash
	chmod 755 /usr/local/bin/cflash

uninstall:
	rm /usr/local/bin/cflash

clean:
	rm cflash