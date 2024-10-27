#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define ARROW  "\u27A4  "

#define BOX_HR "\u2501"
#define BOX_VR "\u2503"
#define BOX_LU "\u250F"
#define BOX_LD "\u2517"
#define BOX_RU "\u2513"
#define BOX_RD "\u251B"
#define BOX_LX "\u2523"
#define BOX_RX "\u252B"

#define BOX_VR_HALF_UP 		"\u2579"
#define BOX_VR_HALF_DOWN 	"\u257B"


/* prints outs the DH MM YS as the box title */
// #define BOX_TITLE
// #define BOX_TOP_SPACE

/*
	uncomment if you want date to include and use year zero
	example 24.5.0 is valid date format
*/
// #define YEAR_ZERO

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

bool leap_year(int year);
int days_in_month(int month, int year);
bool date_validate(struct dts date_s);
void date_update(struct dts* date_s, int days);
void date_increase(struct dts* date_s, int days);
void date_decrease(struct dts* date_s, int days);

bool in_arr(int n, const int* arr, size_t arr_size);
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
				if (strchr(argv[i], 't')) main_dts.pref |= time_took;
				if (strchr(argv[i], 'a')) main_dts.pref |= both_dates;
				if (strchr(argv[i], 'y')) main_dts.pref |= year_zero;
				if (strchr(argv[i], 'k')) main_dts.pref |= weekday;
				if (strchr(argv[i], 'f')) main_dts.pref |= full_weekday;
				if (strchr(argv[i], 'b')) main_dts.pref |= left_side_only;
			}
			else {
				strcat(to_add_input, argv[i]);
			}
		}
	}

	if (main_dts.pref & left_side_only) {
		printf(BOX_LU);
		printf(BOX_HR "\n");
	}

	if (strlen(to_add_input) < 2) {
		printf(BOX_VR " DATE " ARROW );
		fgets(input_str, 30, stdin);
	}

	if (strlen(input_str) >= 2) {
		int sc = sscanf(input_str,
			"%2d%*[^0123456789]%2d%*[^0123456789]%d%*[^0123456789]%2d%*[^0123456789]%2d%*[^0123456789]%2d",
			&main_dts.day, &main_dts.month, &main_dts.year,
			&main_dts.hours, &main_dts.minutes, &main_dts.seconds
		);
		if (sc != 1) {
			printf(BOX_VR " BAD FORMAT\n");
			return -1;
		}
	}
	else {
		time_t rawtime;
		struct tm* timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		from_tm_to_dts(&main_dts, timeinfo);
	}

	if (!date_validate(main_dts) || !time_validate(main_dts)) {
		printf(BOX_VR " BAD FORMAT\n");
		return 1;
	}

	if (strlen(to_add_input) < 2) {
		printf(BOX_VR " ADD  " ARROW);
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
		printf(BOX_VR " TIME " ARROW " %ldns\n", end.tv_nsec - start.tv_nsec);
	}

	if (main_dts.pref & both_dates) {
		box_date_time(main_dts);
	}
	box_date_time(new_dts);

	if (main_dts.pref & left_side_only) {
		printf(BOX_LD BOX_HR "\n");
	}

	return 0;
}

/////////////////////////////////////////////////////////

bool in_arr(int n, const int* arr, size_t arr_size) {
	for (size_t i = 0; i < arr_size; i++) {
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
	static char* weekdays[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	return weekdays[wd];
}

/////////////////////////////////////////////////////////

void box_date_time(struct dts dts_s) {

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
			dts_s.hours, dts_s.minutes, dts_s.seconds
		);
		strcat(date_string, tmp_str);
	}
	else {
		strcat(date_string, " ");
	}

	int hr_max = (strlen(date_string) < 10) ? 10 : strlen(date_string);

	if (!(dts_s.pref & left_side_only)) {
		printf(BOX_LU);
		for (int i = 0; i < hr_max; i++)
			printf(BOX_HR);
		printf(BOX_RU "\n");
	}

#if defined(BOX_TITLE)
	if (!(dts_s.pref & left_side_only)) {
		printf("%s DH MM YS",
			BOX_VR
		);

		for (int i = 0; i < (hr_max - 9); i++)
			printf(" ");

		if (!(dts_s.pref & left_side_only))
			printf(BOX_VR);
		printf("\n");
	}
#endif

#if defined(BOX_TITLE) || defined(BOX_TOP_SPACE)
	if (!(dts_s.pref & left_side_only)) {
		printf(BOX_LX);
		for (int i = 0; i < hr_max; i++)
			printf(BOX_HR);
		printf(BOX_RX "\n");
	}
#endif

	printf(BOX_VR "%s", date_string);

	for (int i = 0; i < (int)(hr_max - strlen(date_string)); i++)
		printf(" ");

	if (!(dts_s.pref & left_side_only)) {
		printf(BOX_VR);
	}
	printf("\n");

	if (!(dts_s.pref & left_side_only)) {
		printf(BOX_VR " %02d:%02d:%02d",
			dts_s.hours, dts_s.minutes, dts_s.seconds
		);

		for (int i = 0; i < (hr_max - 9); i++)
			printf(" ");
	}


	if (!(dts_s.pref & left_side_only)) {
		printf(BOX_VR "\n");
	}
	
	if (!(dts_s.pref & left_side_only)) {
		printf(BOX_LD);
		for (int i = 0; i < hr_max; i++)
			printf(BOX_HR);
		printf(BOX_RD "\n");
	}
}

/////////////////////////////////////////////////////////

void tokenize_input(char* inp_str, int* days, int* seconds) {

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

int days_in_month(int month, int year) {
	switch (month) {
	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
		return 31;
	case 4: case 6: case 9: case 11:
		return 30;
	case 2:
		if (leap_year(year)) return 29;
		return 28;
	}
	return 0;
}

/////////////////////////////////////////////////////////

bool date_validate(struct dts date_s) {
	if (date_s.day <= 0
			|| date_s.day > days_in_month(date_s.month, date_s.year)
			|| !days_in_month(date_s.month, date_s.year))
		return false;

	if (!(date_s.pref & year_zero) && date_s.year == 0) return false;

	return true;
}

/////////////////////////////////////////////////////////

void date_update(struct dts* date_s, int days) {
	(days > 0) ? date_increase(date_s, days) : date_decrease(date_s, -days);
}

/////////////////////////////////////////////////////////

bool leap_year(int year) {
	return (year % 400 == 0) || ((year % 100 != 0) && (year % 4 == 0));
}

/////////////////////////////////////////////////////////

void date_increase(struct dts* date_s, int days) {
	for (int i = 0; i < days; i++) {

		if (++date_s->day > days_in_month(date_s->month, date_s->year)) {
			date_s->day = 1;
			if (++date_s->month > 12) {
				date_s->month = 1;
				date_s->year++;
			}
		}

	}
}

/////////////////////////////////////////////////////////

void date_decrease(struct dts* date_s, int days) {
	for (int i = 0; i < days; i++) {

		if (!--date_s->day) {
			if (!--date_s->month) {
				date_s->month = 12;
				date_s->year--;

				#if !defined(YEAR_ZERO)
				if (date_s->year == 0)
					date_s->year--;
				#endif
			}
			date_s->day = days_in_month(date_s->month, date_s->year);
		}

	}
}

/////////////////////////////////////////////////////////
