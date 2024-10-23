
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <time.h>

int countWords(const char *sentence) {
    regex_t regex;
    regmatch_t matches[1];

    if (regcomp(&regex, "\\b\\w+\\b", REG_EXTENDED) != 0) {
        return -1; // Greška pri kompajliranju
    }

    int count = 0;
    const char *ptr = sentence;

    while (regexec(&regex, ptr, 1, matches, 0) == 0) {
        count++;
        ptr += matches[0].rm_eo; // Pomeri pokazivač na kraj pronađene reči
    }

    regfree(&regex);
    return count;
}

void trimSpaces(char *str) {
    char *end;

    end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\t')) end--;
    *(end + 1) = '\0';

    while (*str == ' ' || *str == '\n' || *str == '\t') str++;
}

char *readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Ne mogu otvoriti fajl");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, length, file);
    buffer[length] = '\0'; // Dodaj null terminator

    fclose(file);
    return buffer;
}

int main() {
    const char *filename = "test.txt"; // Ime fajla
    char *content = readFile(filename);

    if (content) {

		struct timespec res;
	    clock_gettime(CLOCK_REALTIME,&res);

        trimSpaces(content);
        int wordCount = countWords(content);
    

		struct timespec end;
	    clock_gettime(CLOCK_REALTIME,&end);

	    printf("TIME: %lu\n", end.tv_nsec - res.tv_nsec);
        printf("Broj reči: %d\n", wordCount);
        free(content); // Oslobodi memoriju
    }

    return 0;
}
