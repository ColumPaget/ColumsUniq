FLAGS=-g -Wall
all: common.o uniq

common.o: common.c
	gcc $(FLAGS) -c common.c

uniq: uniq.c common.o
	gcc $(FLAGS) -ocuniq uniq.c common.o

clean:
	@rm -f *.o cuniq

check:
	@./check.sh

install:
	cp cuniq $(DESTDIR)$(PREFIX)/bin

