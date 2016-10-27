CURDIR = $(shell pwd)
SRCDIR = $(CURDIR)/src
BINDIR = $(CURDIR)/bin
OUTFILE = $(BINDIR)/sheet

CC = g++ -O0
DEBUGFLAGS = -ggdb3 -O0
DEBUGGER = gdb -q
CFLAGS = -Wall -std=c++11
LFLAGS = -Wall

HEADERS=$(wildcard $(SRCDIR)/*.h)
SOURCES=$(wildcard $(SRCDIR)/*.cpp)

ARGS=-w 256 -h 256 -o my_sheet ~/Desktop/bug_1.36x36pi ~/Desktop/bug_3.4x4pi ~/Desktop/bug_0.24x24pi

b: build
	

$(OUTFILE): $(HEADERS) $(SOURCES)
	@$(CC) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $(OUTFILE)

build: $(OUTFILE)
	@

run: $(OUTFILE)
	@$(OUTFILE) $(ARGS) ; cp my_sheet.256x256pi ~/Desktop/doside/assets/atlas.256x256pi

$(OUTFILE).dSYM: $(HEADERS) $(SOURCES)
	@$(CC) $(DEBUGFLAGS) $(CFLAGS) $(LFLAGS) $(SOURCES) -o $(OUTFILE)

debug: $(OUTFILE).dSYM
	@$(DEBUGGER) --args $(OUTFILE) $(ARGS)

clean:
	@for f in $(OUTFILE) $(OUTFILE).dSYM ; do if [ -d $$f ] ; then rm -r $$f ; elif [ -f $$f ] ; then rm $$f ; fi ; done

kill:
	@killAll $(OUTFILE)

