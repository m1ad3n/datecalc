#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOX_HR "\u2501"
#define BOX_VR "\u2503"
#define BOX_LU "\u250F"
#define BOX_LD "\u2517"
#define BOX_RU "\u2513"
#define BOX_RD "\u251B"
#define BOX_LX "\u2523"
#define BOX_RX "\u252B"

#define BOX_TITLE

struct dts {
	int seconds;
	int minutes;
	int hours;

	int day;
	int month;
	int year;
};

void time_debug(struct dts time_s);
void time_update(struct dts* time_s, int secs);
void time_increase(struct dts* time_s, int secs);
void time_decrease(struct dts* time_s, int secs);

void date_debug(struct dts date_s);
void date_update(struct dts* date_s, int days);
void date_increase(struct dts* date_s, int days);
void date_decrease(struct dts* date_s, int days);

int how_many_digits(int n);
int string_len(char* s);
int string_contains(char* s, char ch);

void box_date_time(struct dts dts_s, int year_digits);
void tokenize_input(char* inp_str, int* days, int* seconds);
void from_tm_to_dts(struct dts* dts_s, struct tm* tm_s);

int main(int argc, char* argv[]) {

	struct dts main_dts = {0};

	char input_str[30] = {0};
	char to_add_input[50] = {0};

	char display_time = 0;
	char display_both_dates = 0;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (string_contains(argv[i], 't')) {
				display_time++;
			}
			if (string_contains(argv[i], 'b')) {
				display_both_dates++;
			}
			else {
				strcat(to_add_input, argv[i]);
			}
		}
	}

	if (string_len(to_add_input) < 2) {
		printf("%s date > ", BOX_VR);
		fgets(input_str, 30, stdin);
	}

	if (string_len(input_str) >= 2) {
		sscanf(input_str,
			"%2d.%2d.%d %2d:%2d:%2d",
			&main_dts.day, &main_dts.month, &main_dts.year,
			&main_dts.hours, &main_dts.minutes, &main_dts.seconds
		);
	}
	else {
		time_t rawtime;
		struct tm* timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		from_tm_to_dts(&main_dts, timeinfo);
	}

	if (!string_len(to_add_input)) {
		printf("%s add > ", BOX_VR);
		fgets(to_add_input, 50, stdin);	
	}

	int days_to_add = 0, secs_to_add = 0;
	tokenize_input(to_add_input, &days_to_add, &secs_to_add);

	if (display_both_dates)
		box_date_time(main_dts, how_many_digits(main_dts.year));

	struct timespec start, end;
	if (display_time) {
		clock_gettime(CLOCK_REALTIME, &start);
	}

	time_update(&main_dts, secs_to_add);
	date_update(&main_dts, days_to_add);

	if (display_time) {
		clock_gettime(CLOCK_REALTIME, &end);
		printf("%s TIME: %dns\n", BOX_VR, end.tv_nsec - start.tv_nsec);
	}

	box_date_time(main_dts, how_many_digits(main_dts.year));
	return 0;
}

int string_len(char* s) {
	int count = 0;
	while (*s != '\0') {
		count++;
		s++;
	}
	return count;
}

int string_contains(char* s, char ch) {
	while (*s != '\0')
		if (*s++ == ch) return 1;
	return 0;
}

int how_many_digits(int n) {
	if (n < 0) n = ~n + 1;
	int count = 0;
	while (n) {
		n /= 10;
		count++;
	}
	return count;
}

void from_tm_to_dts(struct dts* dts_s, struct tm* tm_s) {
	dts_s->seconds = tm_s->tm_sec;
	dts_s->minutes = tm_s->tm_min;
	dts_s->hours = tm_s->tm_hour;
	dts_s->day = tm_s->tm_mday;
	dts_s->month = tm_s->tm_mon + 1;
	dts_s->year = tm_s->tm_year + 1900;
}

void box_date_time(
	struct dts dts_s,
	int year_digits
) {
	int hr_max;
	if (year_digits == 0) year_digits++;
	if (dts_s.year < 0) year_digits += 3;

	if (year_digits <= 2) hr_max = 10;
	else hr_max = 9 + year_digits - 1;

	fputs(BOX_LU, stdout);
	for (int i = 0; i < hr_max; i++)
		fputs(BOX_HR, stdout);
	puts(BOX_RU);

#if defined(BOX_TITLE)
	printf("%s DH MM YS",
		BOX_VR
	);

	for (int i = 0; i < (hr_max - 9); i++)
		putc(32, stdout);
	puts(BOX_VR);
#endif

#if defined(BOX_TITLE) || defined(BOX_TOP_SPACE)
	fputs(BOX_LX, stdout);
	for (int i = 0; i < hr_max; i++)
		fputs(BOX_HR, stdout);
	puts(BOX_RX);
#endif

	printf("%s %02d.%02d.",
		BOX_VR,
		dts_s.day, dts_s.month
	);

	if (dts_s.year < 0) printf("%d BC", ~dts_s.year + 1);
	else printf("%d", dts_s.year);

	for (int i = 0; i < (hr_max - (7 + year_digits)); i++)
		putc(32, stdout);
	puts(BOX_VR);

	printf("%s %02d:%02d:%02d",
		BOX_VR,
		(int)dts_s.hours, (int)dts_s.minutes, (int)dts_s.seconds
	);

	for (int i = 0; i < (hr_max - 9); i++)
		putc(32, stdout);
	puts(BOX_VR);

	fputs(BOX_LD, stdout);
	for (int i = 0; i < hr_max; i++)
		fputs(BOX_HR, stdout);
	puts(BOX_RD);
}

void tokenize_input (
	char* inp_str,
	int* days, int* seconds
) {

	int sum = 0;
	char op = 0;

	while (*inp_str != '\0') {

		if (*inp_str >= 'a' && *inp_str <= 'z') {
			if (op) sum = ~sum + 1;
		} 

		switch (*inp_str) {
		case 'w':
			*days += (sum * 7);
			break;
		case 'd':
			*days += sum;
			break;
		case 'h':
			*seconds += (sum * 3600);
			break;
		case 'm':
			*seconds += (sum * 60);
			break;
		case 's':
			*seconds += sum;
			break;
		}

		if (*inp_str >= '0' && *inp_str <= '9') {
			sum = (sum * 10) + (int)(*inp_str - '0');
		}
		else if (*inp_str == '-') {
			op = *inp_str;
		}
		else if (*inp_str != ' ') {
			sum = 0;
			op = 0;
		}

		inp_str++;
	}
}

void time_debug(struct dts time_s) {
	printf("%02d:%02d:%02d\n", time_s.hours, time_s.minutes, time_s.seconds);
}

void time_increase(struct dts* time_s, int secs) {

	//
	// REALLY SLOW VERSION (secs=1_000_000 -> takes ~5_000_000 nano seconds)
	// 
	// int carry_days = 0;
	// for (int i = 0; i < secs; i++) {
	// 	time_s->seconds++;
	// 	if (time_s->seconds > 59) {
	// 		time_s->seconds = 0;
	// 		time_s->minutes++;
	// 		if (time_s->minutes > 59) {
	// 			time_s->hours++;
	// 			time_s->minutes = 0;
	// 			if (time_s->hours > 23) {
	// 				time_s->hours = 0;
	// 				carry_days++;
	// 			}
	// 		}
	// 	}
	// }

	//
	// FAST VERSION (secs=1_000_000 -> takes ~400 nano seconds)
	//

	int carry_min = (int)((time_s->seconds + secs) / 60);
	time_s->seconds = (time_s->seconds + secs) % 60;

	int carry_hours = (int)((time_s->minutes + carry_min) / 60);
	time_s->minutes = (time_s->minutes + carry_min) % 60;

	int carry_days = (int)((time_s->hours + carry_hours) / 24);
	time_s->hours = (time_s->hours + carry_hours) % 24;

	date_increase(time_s, carry_days);
}

void time_decrease(struct dts* time_s, int secs) {
	
	//
	// REALLY SLOW VERSION (secs=1_000_000 -> takes ~5_000_000 nano seconds)
	//
	// int carryd = 0;
	// for (int i = 0; i < secs; i++) {
	// 	time_s->seconds--;
	// 	if (time_s->seconds < 0) {
	// 		time_s->seconds = 59;
	// 		time_s->minutes--;

	// 		if (time_s->minutes < 0) {
	// 			time_s->minutes = 59;
	// 			time_s->hours--;

	// 			if (time_s->hours < 0) {
	// 				time_s->hours = 23;
	// 				carryd--;
	// 			} 
	// 		}
	// 	}
	// }

	//
	// FAST VERSION (secs=1_000_000 -> takes ~7000 nano seconds)
	//
	int carryd = 0;
	int total_seconds = time_s->hours * 3600 + time_s->minutes * 60 + time_s->seconds;

    total_seconds -= secs;
    if (total_seconds < 0) {
        carryd = (abs(total_seconds) / 86401) + 1;
        total_seconds = (86400 + (total_seconds % 86400)) % 86400;
    } else {
        carryd = 0;
    }
    
    time_s->hours = total_seconds / 3600;
    time_s->minutes = (total_seconds % 3600) / 60;
    time_s->seconds = total_seconds % 60;

    if (time_s->hours >= 24) {
        carryd += time_s->hours / 24;
        time_s->hours = time_s->hours % 24;
    }

	date_decrease(time_s, carryd);
}

void time_update(struct dts* time_s, int secs) {
	return (secs >= 0) ? time_increase(time_s, secs) : time_decrease(time_s, ~secs + 1);
}

void date_debug(struct dts date_s) {
	printf("%02d.%02d.%d\n", date_s.day, date_s.month, date_s.year);
}

void date_update(struct dts* date_s, int days) {
	(days >= 0) ? date_increase(date_s, days) : date_decrease(date_s, ~days + 1);
}

void date_increase(struct dts* date_s, int days) {
	for (int i = 0; i < days; i++) {
		date_s->day++;
		if (date_s->day < 28) continue;

		switch (date_s->month) {
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:

			if (date_s->day > 31) {
				date_s->day = 1;
				if (date_s->month == 12) {
					date_s->year++;
					date_s->month = 1;
				}
				else {
					date_s->month++;
				}
			}
			break;

		case 4: case 6: case 9: case 11:
			if (date_s->day > 30) {
				date_s->day = 1;
				date_s->month++;
			}
			break;

		case 2:
			if ((date_s->year % 400 == 0) ||
				((date_s->year % 100 != 0) &&
				(date_s->year % 4 == 0)))
			{
				if (date_s->day > 29) {
					date_s->day = 1;
					date_s->month++;
				}
			}
			else {
				if (date_s->day > 28) {
					date_s->day = 1;
					date_s->month++;
				}
			}
			break;
		}
	}
}

void date_decrease(struct dts* date_s, int days) {
	for (int i = 0; i < days; i++) {
		date_s->day--;
		if (date_s->day > 0) continue;

		switch (date_s->month) {
		case 1: case 2: case 4: case 6: case 8: case 9: case 11:
			date_s->day = 31;
			if (date_s->month == 1) {
				date_s->year--;
				date_s->month = 12;
			}
			else {
				date_s->month--;
			}
			break;

		case 5: case 7: case 10: case 12:
			date_s->day = 30;
			date_s->month--;
			break;

		case 3:
			date_s->day = 28;
			date_s->month = 2;
			if ((date_s->year % 400 == 0) ||
				((date_s->year % 100 != 0) &&
				(date_s->year % 4 == 0)))
			{
				date_s->day++;
			}
			break;
		}
	}
}
