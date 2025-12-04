PREFIX = /usr/local

src_Linux = $(wildcard src/linux/*.c)
src_IRIX = $(wildcard src/irix/*.c)
src_IRIX64 = $(wildcard src/irix/*.c)

src = $(wildcard src/*.c) $(src_$(shell uname -s))
obj = $(src:.c=.o)
dep = $(src:.c=.d)
bin = xmon

CFLAGS = -std=gnu89 -pedantic -Wall -g -Isrc -MMD
LDFLAGS = -lX11

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)

.PHONY: cleandep
cleandep:
	rm -f $(dep)

.PHONY: install
install: $(bin)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(bin) $(DESTDIR)$(PREFIX)/bin/$(bin)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(bin)
