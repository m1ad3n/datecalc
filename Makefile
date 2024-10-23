all: bin/txtinfo bin/genpassw

bin/%: src/%.c
	gcc -Wall -o $@ $<

clean:
	@rm -rvf bin/**