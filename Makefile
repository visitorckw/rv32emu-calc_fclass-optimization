CC = gcc
CFLAGS = -O2

all: main

mainOBJS := main.o

deps := $(OBJS:%.o=.%.o.d)

main: $(mainOBJS)
	$(CC) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

test: main
	@./main

clean:
	rm -f $(mainOBJS) $(deps) *~ main
	rm -rf *.dSYM

-include $(deps)

