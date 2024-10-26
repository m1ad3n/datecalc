CFLAGS	:= -Wall -Wextra -Wno-format -O3

all: datecalc

%: %.c
	gcc $(CFLAGS) -s -o $@ $<
