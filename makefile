CURDIR = $(shell pwd)
SRCDIR = $(CURDIR)/src
BINDIR = $(CURDIR)/bin
OUTFILE = $(BINDIR)/asset_gen.out

CC = g++ -O3
DEBUGFLAGS = -ggdb3 -O3
DEBUGGER = gdb -q
CFLAGS = -Wall -std=c++11
LFLAGS = -Wall

HEADERS=$(wildcard $(SRCDIR)/*.h)
SOURCES=$(wildcard $(SRCDIR)/*.cpp)

b: build
	

$(OUTFILE): $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $(OUTFILE)

build: $(OUTFILE)
	@

run: $(OUTFILE)
	@$(OUTFILE)

$(OUTFILE).dSYM: $(HEADERS) $(SOURCES)
	@$(CC) $(DEBUGFLAGS) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $(OUTFILE)

debug: $(OUTFILE).dSYM
	@$(DEBUGGER) $(OUTFILE)

clean:
	@for f in $(OUTFILE) $(OUTFILE).dSYM ; do if [ -d $$f ] ; then rm -r $$f ; elif [ -f $$f ] ; then rm $$f ; fi ; done

kill:
	@killAll $(OUTFILE)

