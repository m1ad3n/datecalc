#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ASCII_MIN 33
#define ASCII_MAX 122

static int get_rand_char() {
	return rand() % (ASCII_MAX - ASCII_MIN + 1) + ASCII_MIN;
}

int main(int argc, char* argv[]) {
	struct timespec res;
    clock_gettime(CLOCK_REALTIME,&res);
    
	srand(res.tv_nsec);
	if (argc < 2) {
		perror("not enough arguments");
		abort();
	}
	for (int i = 0; i < atoi(argv[1]); i++) {
		int ch = get_rand_char();
		if (
			ch == 39 ||
			ch == 44 ||
			ch == 96
		)
			continue; 
		putc(ch, stdout);
	}

	putc(10, stdout);
	return 0;
}