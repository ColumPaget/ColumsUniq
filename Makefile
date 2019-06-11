FLAGS=-g -Wall
all: common.o uniq

common.o: common.c
	gcc $(FLAGS) -c common.c

uniq: uniq.c common.o
	gcc $(FLAGS) -ocuniq uniq.c common.o

clean:
	@rm -f *.o cut uniq

check:
	@./check.sh

install:
	cp cuniq $(DESTDIR)$(PREFIX)/bin

test: libUseful.so
	-echo "No tests written yet"
