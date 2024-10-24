CFLAGS	:= -Wall -Wextra -Wno-format
TEMP	:= $(subst src,bin,$(wildcard src/*.c))
TARGETS	:= $(TEMP:.c=.elf)

all: $(TARGETS)

bin/%.elf: src/%.c
	gcc $(CFLAGS) -o $@ $<

clean:
	@rm -rvf bin/**
