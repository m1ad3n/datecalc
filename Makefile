TEMP	:= $(subst src,bin,$(wildcard src/*.c))
TARGETS	:= $(TEMP:.c=.elf)

all: $(TARGETS)

bin/%.elf: src/%.c
	gcc -Wall -o $@ $<

clean:
	@rm -rvf bin/**