#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define ARROW  "\u27A4"

#define BOX_HR "\u2501"
#define BOX_VR "\u2503"
#define BOX_LU "\u250F"
#define BOX_LD "\u2517"
#define BOX_RU "\u2513"
#define BOX_RD "\u251B"
#define BOX_LX "\u2523"
#define BOX_RX "\u252B"

#define BOX_VR_HALF_UP "\u2579"
#define BOX_VR_HALF_DOWN "\u257B"

// #define BOX_TITLE
// #define BOX_TOP_SPACE

/////////////////////////////////////////////////////////

struct dts {
	int seconds;
	int minutes;
	int hours;

	int day;
	int month;
	int year;

	char pref;
};

enum dts_prefs {
	year_zero = 1,
	weekday = 2,
	time_took = 4,
	both_dates = 8,
	one_line = 16,
	full_weekday = 32,
	left_side_only = 64
};

/////////////////////////////////////////////////////////

bool time_validate(struct dts time_s);
void time_update(struct dts* time_s, int secs);
void time_increase(struct dts* time_s, int secs);
void time_decrease(struct dts* time_s, int secs);

bool date_validate(struct dts date_s);
void date_update(struct dts* date_s, int days);
void date_increase(struct dts* date_s, int days);
void date_decrease(struct dts* date_s, int days);

int how_many_digits(int n);
int string_contains(char* s, char ch);
bool in_arr(int n, int* arr, int arr_size);

void box_date_time(struct dts dts_s);
void tokenize_input(char* inp_str, int* days, int* seconds);
void from_tm_to_dts(struct dts* dts_s, struct tm* tm_s);

/////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

	struct dts main_dts = {0};

	char input_str[30] = {0};
	char to_add_input[50] = {0};

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			if (*argv[i] == '-') {
				if (string_contains(argv[i], 't')) main_dts.pref |= time_took;
				if (string_contains(argv[i], 'a')) main_dts.pref |= both_dates;
				if (string_contains(argv[i], 'y')) main_dts.pref |= year_zero;
				if (string_contains(argv[i], 'k')) main_dts.pref |= weekday;
				if (string_contains(argv[i], 'f')) main_dts.pref |= full_weekday;
				if (string_contains(argv[i], 'b')) main_dts.pref |= left_side_only;
			}
			else {
				strcat(to_add_input, argv[i]);
			}
		}
	}

	if (main_dts.pref & left_side_only) {
		fputs(BOX_LU, stdout);
		puts(BOX_HR);
	}

	if (strlen(to_add_input) < 2) {
		printf("%s DATE %s  ", BOX_VR, ARROW);
		fgets(input_str, 30, stdin);
	}

	if (strlen(input_str) >= 2) {
		sscanf(input_str,
			"%2d%*[^0123456789]%2d%*[^0123456789]%d%*[^0123456789]%2d%*[^0123456789]%2d%*[^0123456789]%2d",
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

	if (!date_validate(main_dts) || !time_validate(main_dts)) {
		printf("%s BAD FORMAT\n", BOX_VR);
		return 1;
	}

	if (strlen(to_add_input) < 2) {
		printf("%s ADD  %s  ", BOX_VR, ARROW);
		fgets(to_add_input, 50, stdin);	
	}

	int days_to_add = 0, secs_to_add = 0;
	tokenize_input(to_add_input, &days_to_add, &secs_to_add);

	struct timespec start, end;
	if (main_dts.pref & time_took) {
		clock_gettime(CLOCK_REALTIME, &start);
	}

	struct dts new_dts = main_dts; 
	date_update(&new_dts, days_to_add);
	time_update(&new_dts, secs_to_add);

	if (main_dts.pref & time_took) {
		clock_gettime(CLOCK_REALTIME, &end);
		printf("%s TIME %s  %ldns\n", BOX_VR, ARROW, end.tv_nsec - start.tv_nsec);
	}

	if (main_dts.pref & both_dates) {
		box_date_time(main_dts);
	}
	box_date_time(new_dts);

	if (main_dts.pref & left_side_only) {
		fputs(BOX_LD, stdout);
		puts(BOX_HR);
	}

	return 0;
}

/////////////////////////////////////////////////////////

int string_contains(char* s, char ch) {
	while (*s != '\0')
		if (*s++ == ch) return 1;
	return 0;
}

/////////////////////////////////////////////////////////

int how_many_digits(int n) {
	if (n < 0) n = ~n + 1;
	int count = 0;
	while (n) {
		n /= 10;
		count++;
	}
	return count;
}

/////////////////////////////////////////////////////////

bool in_arr(int n, int* arr, int arr_size) {
	for (int i = 0; i < arr_size; i++) {
		if (arr[i] == n) {
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////

void from_tm_to_dts(struct dts* dts_s, struct tm* tm_s) {
	dts_s->seconds = tm_s->tm_sec;
	dts_s->minutes = tm_s->tm_min;
	dts_s->hours = tm_s->tm_hour;
	dts_s->day = tm_s->tm_mday;
	dts_s->month = tm_s->tm_mon + 1;
	dts_s->year = tm_s->tm_year + 1900;
}

char* get_weekday(int wd) {
	char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	return weekdays[wd];
}

/////////////////////////////////////////////////////////

void box_date_time(
	struct dts dts_s
) {

	char date_string[50] = {0};
	char tmp_str[30] = {0};
	date_string[0] = 32;

	if ((dts_s.pref & weekday) || (dts_s.pref & full_weekday)) {
		int d = dts_s.day, m = dts_s.month, y = dts_s.year;
		int wd = (d += m < 3 ? y-- : y - 2, 23 * m / 9 + d + 4 + y/4 - y/100 + y/400) % 7; 
		strncat(date_string, get_weekday(wd), (dts_s.pref & full_weekday) ? 9 : 3);
		strcat(date_string, ", ");
	}

	sprintf(tmp_str, "%02d.%02d.%d",
		dts_s.day, dts_s.month, (dts_s.year < 0) ? ~dts_s.year + 1 : dts_s.year);
	strcat(date_string, tmp_str);
	if (dts_s.year < 0) strcat(date_string, " BC");

	if (dts_s.pref & left_side_only) {
		sprintf(tmp_str, ", %02d:%02d:%02d",
			(int)dts_s.hours, (int)dts_s.minutes, (int)dts_s.seconds
		);
		strcat(date_string, tmp_str);
	}
	else {
		strcat(date_string, " ");
	}

	int hr_max = (strlen(date_string) < 10) ? 10 : strlen(date_string);

	if (!(dts_s.pref & left_side_only)) {
		fputs(BOX_LU, stdout);
		for (int i = 0; i < hr_max; i++)
			fputs(BOX_HR, stdout);
		puts(BOX_RU);
	}

#if defined(BOX_TITLE)
	printf("%s DH MM YS",
		BOX_VR
	);

	for (int i = 0; i < (hr_max - 9); i++)
		putc(32, stdout);

	if (!(dts_s.pref & left_side_only))
		fputs(BOX_VR, stdout);
	putchar(10);
#endif

#if defined(BOX_TITLE) || defined(BOX_TOP_SPACE)
	if (!(dts_s.pref & left_side_only)) {
		fputs(BOX_LX, stdout);
		for (int i = 0; i < hr_max; i++)
			fputs(BOX_HR, stdout);
		fputs(BOX_RX, stdout);
	}
	putchar(10)
#endif

	fputs(BOX_VR, stdout);
	fputs(date_string, stdout);

	for (int i = 0; i < (int)(hr_max - strlen(date_string)); i++)
		putchar(32);

	if (!(dts_s.pref & left_side_only)) {
		fputs(BOX_VR, stdout);
	}
	putchar(10);

	if (!(dts_s.pref & left_side_only)) {
		printf("%s %02d:%02d:%02d",
			BOX_VR,
			(int)dts_s.hours, (int)dts_s.minutes, (int)dts_s.seconds
		);

		for (int i = 0; i < (hr_max - 9); i++)
			putc(32, stdout);
	}


	if (!(dts_s.pref & left_side_only)) {
		puts(BOX_VR);
	}
	
	if (!(dts_s.pref & left_side_only)) {
		fputs(BOX_LD, stdout);
		for (int i = 0; i < hr_max; i++)
			fputs(BOX_HR, stdout);
		puts(BOX_RD);
	}
}

/////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////

bool time_validate(struct dts time_s) {
	if (time_s.hours < 0 || time_s.hours >= 24) return false;
	if (time_s.minutes < 0 || time_s.minutes >= 60) return false;
	if (time_s.seconds < 0 || time_s.seconds >= 60) return false;
	return true;
}

/////////////////////////////////////////////////////////

void time_increase(struct dts* time_s, int secs) {
	int carry_min = (int)((time_s->seconds + secs) / 60);
	time_s->seconds = (time_s->seconds + secs) % 60;

	int carry_hours = (int)((time_s->minutes + carry_min) / 60);
	time_s->minutes = (time_s->minutes + carry_min) % 60;

	int carry_days = (int)((time_s->hours + carry_hours) / 24);
	time_s->hours = (time_s->hours + carry_hours) % 24;

	date_increase(time_s, carry_days);
}

/////////////////////////////////////////////////////////

void time_decrease(struct dts* time_s, int secs) {
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

/////////////////////////////////////////////////////////

void time_update(struct dts* time_s, int secs) {
	(secs >= 0) ? time_increase(time_s, secs) : time_decrease(time_s, ~secs + 1);
}

/////////////////////////////////////////////////////////

bool month_with_31(int month) {
	int months[7] = {1, 3, 5, 7, 8, 10, 12};
	return in_arr(month, months, 7);
}

/////////////////////////////////////////////////////////

bool date_validate(struct dts date_s) {
	if (date_s.day <= 0 || date_s.day > 31) return false;
	if (date_s.month <= 0 || date_s.month > 12) return false;
	if (!month_with_31(date_s.month) && date_s.day > 30) return false;

	if (!(date_s.pref & year_zero) && date_s.year == 0) return false;

	return true;
}

/////////////////////////////////////////////////////////

void date_update(struct dts* date_s, int days) {
	(days > 0) ? date_increase(date_s, days) : date_decrease(date_s, ~days + 1);
}

/////////////////////////////////////////////////////////

void date_increase(struct dts* date_s, int days) {
	for (int i = 0; i < days; i++) {
		date_s->day++;
		if (date_s->day < 28) continue;

		if (date_s->month == 2) {
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
		}
		else if (month_with_31(date_s->month)) {
			if (date_s->day > 31) {
				date_s->day = 1;
				if (date_s->month == 12) {
					date_s->year++;
					date_s->month = 1;

					#if !defined(YEAR_ZERO)
					if (date_s->year == 0)
						date_s->year++;
					#endif
				}
				else {
					date_s->month++;
				}
			}
		}
		else {
			if (date_s->day > 30) {
				date_s->day = 1;
				date_s->month++;
			}
		}
	}
}

/////////////////////////////////////////////////////////

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

				#if !defined(YEAR_ZERO)
				if (date_s->year == 0)
					date_s->year--;
				#endif
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

/////////////////////////////////////////////////////////
