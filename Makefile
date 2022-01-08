CFLAGS=--std=gnu17 -Wall -pedantic -Isrc/ -ggdb -Wextra -Werror -DDEBUG
BUILDDIR=build
SRCDIR=src
CC=gcc

all: $(BUILDDIR)/mem.o $(BUILDDIR)/util.o $(BUILDDIR)/mem_debug.o $(BUILDDIR)/main.o $(BUILDDIR)/5th_test.o $(BUILDDIR)/4th_test.o $(BUILDDIR)/3rd_test.o $(BUILDDIR)/2nd_test.o $(BUILDDIR)/1st_test.o
	$(CC) -o $(BUILDDIR)/main $^

build:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/mem.o: $(SRCDIR)/mem.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/mem_debug.o: $(SRCDIR)/mem_debug.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/util.o: $(SRCDIR)/util.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/1st_test.o: $(SRCDIR)/test/1st_test.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/2nd_test.o: $(SRCDIR)/test/2nd_test.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/3rd_test.o: $(SRCDIR)/test/3rd_test.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/4th_test.o: $(SRCDIR)/test/4th_test.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/5th_test.o: $(SRCDIR)/test/5th_test.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/main.o: $(SRCDIR)/main.c build
	$(CC) -c $(CFLAGS) $< -o $@


test: all $(BUILDDIR)/main
	./$(BUILDDIR)/main

clean:
	rm -rf $(BUILDDIR)

