#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int count_ch(const char* text, int ch, bool rep) {
	int count = 0;
	while (*text != '\0') {
		if (*text == (char)ch) {
			count++;
			if (rep) {
				while (*text == (char)ch)
					text++;
			}
		}
		text++;
	}
	return count;
}

void replace_ch(char* text, int ch, int new_ch) {
	char* tmp = text;
	while (*tmp != '\0') {
		if (*tmp == ch)
			*tmp = new_ch;
		tmp++;
	}
}

int count_words(const char* text) {
	int count = 0;
	while (*text != '\0') {
		if (*text == 32 || *text == 10) {
			bool was = (*text == 32);
			if (*text == 10)
				text++;
			while (*text == 32)
				text++;
			if (!was) {
				while (*text == 32)
					text++;
			}
			count++;
		}
		else
			text++;
	}
	return count;
}

char* trimstr(char** s) {
	char* tmp = *s;
	char* old_start = *s;

	while (*tmp != '\0')
		tmp++;
	tmp--;
	while (*tmp == 32)
		tmp--;
	tmp++;
	*tmp = '\0';

	tmp = *s;
	while (*tmp == 32)
		tmp++;
	*s = tmp;

	return old_start;
}

char* readfile(const char* path, int* filesize) {
	FILE* fp = fopen(path, "r");
	fseek(fp, 0, SEEK_END);
	
	*filesize = ftell(fp);
	char* content = (char*)malloc(*filesize + 1);

	fseek(fp, 0, SEEK_SET);
	for (long i = 0; i < *filesize; i++)
		content[i] = fgetc(fp);
	content[*filesize] = '\0';
	return content;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		perror("not enough arguments");
		abort();
	}

	int size;
	char* content = readfile(argv[1], &size);

	char* must_free = trimstr(&content);
	replace_ch(content, 63, 46);
	replace_ch(content, 33, 46);

	int lines = count_ch(content, 10, false) + 1;
	int sentences = count_ch(content, 46, true);
	int words = count_words(content) + 1;

	printf("CHARACTERS: %d\nLINES: %d\nWORDS: %d\nSENTENCES: %d\n", size, lines, words, sentences);
	
	free(must_free);
	return 0;
}