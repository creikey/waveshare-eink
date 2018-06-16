.PHONY: all clean install generate lib uninstall


BUILDDIR=build
INCLUDEDIR=w_eink
EXNAME=waveink

SRC=$(wildcard *.c)
HEADERS=$(wildcard w_eink/*.h)
CFLAGS = -Wall -I$(INCLUDEDIR)/ -g
TMPOBJS=$(SRC:.c=.o)
OBJS=$(addprefix $(BUILDDIR)/, $(TMPOBJS))

all: $(OBJS) $(HEADERS)
	$(CC) $(OBJS) $(CFLAGS) -o $(EXNAME).out

clean:
	-rm -f $(BUILDDIR)/*.o
	-rm -f $(EXNAME).out
	-rm -f -r out
	-rm -f lib$(EXNAME).a

lib: all
	ar cr lib$(EXNAME).a $(OBJS)

generate: all lib
	mkdir out
	install -s $(EXNAME).out out/$(EXNAME)
	install -d out/lib
	install -m 0644 lib$(EXNAME).a out/lib
	install -d out/include/$(INCLUDEDIR)
	install -m 0644 $(INCLUDEDIR)/* -t out/include/$(INCLUDEDIR)

install: all lib
	sudo install -s $(EXNAME).out /usr/local/bin/$(EXNAME)
	sudo install -m 0644 lib$(EXNAME).a /usr/local/lib
	sudo install -d /usr/local/include/$(INCLUDEDIR)
	sudo install -m 0644 $(INCLUDEDIR)/* -t /usr/local/include/$(INCLUDEDIR)

uninstall:
	sudo rm -f /usr/local/bin/$(EXNAME)
	sudo rm -f /usr/local/lib/lib$(EXNAME).a
	sudo rm -r -f /usr/local/include/$(INCLUDEDIR)/


$(BUILDDIR)/%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@