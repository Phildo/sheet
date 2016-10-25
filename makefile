CURDIR = $(shell pwd)
SRCDIR = $(CURDIR)/src
BINDIR = $(CURDIR)/bin
OUTFILE = $(BINDIR)/asset_gen.out

CC = g++ -O0
DEBUGFLAGS = -ggdb3 -O0
DEBUGGER = gdb -q
CFLAGS = -Wall -std=c++11
LFLAGS = -Wall

HEADERS=$(wildcard $(SRCDIR)/*.h)
SOURCES=$(wildcard $(SRCDIR)/*.cpp)

ARGS=-w 256 -h 256 -o my_sheet ~/Desktop/bug_0.24x24pi ~/Desktop/bug_1.36x36pi

b: build
	

$(OUTFILE): $(HEADERS) $(SOURCES)
	@$(CC) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $(OUTFILE)

build: $(OUTFILE)
	@

run: $(OUTFILE)
	@$(OUTFILE) $(ARGS)

$(OUTFILE).dSYM: $(HEADERS) $(SOURCES)
	@$(CC) $(DEBUGFLAGS) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $(OUTFILE)

debug: $(OUTFILE).dSYM
	@$(DEBUGGER) --args $(OUTFILE) $(ARGS)

clean:
	@for f in $(OUTFILE) $(OUTFILE).dSYM ; do if [ -d $$f ] ; then rm -r $$f ; elif [ -f $$f ] ; then rm $$f ; fi ; done

kill:
	@killAll $(OUTFILE)

