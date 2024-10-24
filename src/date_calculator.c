#include <stdio.h>

#define BOX_HR "\u2501"
#define BOX_VR "\u2503"
#define BOX_LU "\u250F"
#define BOX_LD "\u2517"
#define BOX_RU "\u2513"
#define BOX_RD "\u251B"
#define BOX_LX "\u2523"
#define BOX_RX "\u252B"

// #define BOX_TITLE
// #define BOX_TOP_SPACE

struct time {
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
};

void time_debug(struct time time_s);
int time_increase(struct time* time_s, unsigned int secs);

struct date {
	unsigned short year;
	unsigned char month;
	unsigned char day;
};

void date_debug(struct date date_s);
void date_increase(struct date* date_s, unsigned int days);

int how_many_digits(int n);
void box_date_time(struct date date_s, struct time time_s, int year_digits);
void tokenize_input(char* inp_str, unsigned int* days, unsigned int* seconds);

int main(int argc, char* argv[]) {

	int
		day, month, year,
		seconds, minutes, hours;

	printf("date > ");
	fscanf(stdin,
		"%2d.%2d.%d %2d:%2d:%2d",
		&day, &month, &year,
		&hours, &minutes, &seconds
	);
	getchar();

	char to_add_input[50];
	printf("to add > ");
	fgets(to_add_input, 50, stdin);

	unsigned int days_to_add = 0, secs_to_add = 0;
	tokenize_input(to_add_input, &days_to_add, &secs_to_add);

	struct time main_time = {
		hours, minutes, seconds
	};

	struct date main_date = {
		year, month, day
	};

	int carryd = time_increase(&main_time, secs_to_add);
	date_increase(&main_date, days_to_add + carryd);
	box_date_time(main_date, main_time, how_many_digits(main_date.year));

	return 0;
}

int how_many_digits(int n) {
	int count = 0;
	while (n) {
		n /= 10;
		count++;
	}
	return count;
}

void box_date_time(
	struct date date_s,
	struct time time_s,
	int year_digits
) {
	int hr_max;
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

	printf("%s %02d.%02d.%d",
		BOX_VR,
		date_s.day, date_s.month, date_s.year
	);

	for (int i = 0; i < (hr_max - (7 + year_digits)); i++)
		putc(32, stdout);
	puts(BOX_VR);

	printf("%s %02d:%02d:%02d",
		BOX_VR,
		time_s.hours, time_s.minutes, time_s.seconds
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
	unsigned int* days, unsigned int* seconds
) {

	unsigned int sum = 0;
	while (*inp_str != '\0') {

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
			sum = (sum * 10) + (unsigned int)(*inp_str - '0');
		}
		else if (*inp_str != ' ') {
			sum = 0;
		}

		inp_str++;
	}
}

void time_debug(struct time time_s) {
	printf("%02d:%02d:%02d\n", time_s.hours, time_s.minutes, time_s.seconds);
}

int time_increase(struct time* time_s, unsigned int secs) {

	int carry_min = (int)((time_s->seconds + secs) / 60);
	time_s->seconds = (time_s->seconds + secs) % 60;

	int carry_hours = (int)((time_s->minutes + carry_min) / 60);
	time_s->minutes = (time_s->minutes + carry_min) % 60;

	int carry_days = (int)((time_s->hours + carry_hours) / 24);
	time_s->hours = (time_s->hours + carry_hours) % 24;

	return carry_days;
}

void date_debug(struct date date_s) {
	printf("%02d.%02d.%d\n", date_s.day, date_s.month, date_s.year);
}

void date_increase(struct date* date_s, unsigned int days) {
	unsigned char carrym;
	for (int i = 0; i < days; i++) {
		date_s->day++;
		switch (date_s->month) {
		case 1: case 3: case 5: case 7: case 8: case 10: case 12:
			carrym = (date_s->day % 32) ? 0 : 1;
			if (carrym) date_s->day = 1;
			date_s->month += carrym;
			date_s->year += (int)((date_s->month) / 13);
			date_s->month = (date_s->month % 13) ? date_s->month : 1;
			break;

		case 4: case 6: case 9: case 11:
			carrym = (date_s->day % 31) ? 0 : 1;
			if (carrym) date_s->day = 1;
			date_s->month += carrym;
			break;

		case 2:
			if ((date_s->year % 400 == 0) ||
				((date_s->year % 100 != 0) &&
				(date_s->year % 4 == 0))) {
				carrym = (date_s->day / 30) ? 1 : 0;
				if (carrym) date_s->day = 1;
				date_s->month += carrym;
			}
			else {
				carrym = (date_s->day % 29) ? 0 : 1;
				if (carrym) date_s->day = 1;
				date_s->month += carrym;
			}
		}
	}
}