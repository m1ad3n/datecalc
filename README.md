## datecalc

Overview

datecalc is a command-line tool written in C that allows users to calculate the number of days since a given date or the current date. It also supports calculating future dates by adding a specified number of days or seconds.

Features

* Calculate the number of days from today or any specified date.
* Add days or seconds to a date and return the resulting date.
* Support for different input formats based on user preferences.

Installation

To compile and run the program, you need to have a C compiler installed. Follow these steps:

Clone the repository:

```bash
git clone https://github.com/m1ad3n/datecalc.git
cd datecalc
```

Compile the program:

```bash
make
```

Run the program:

```bash
datecalc
```
Usage

You can use datecalc with various command-line options:

To add days or seconds to todays date:

```bash
datecalc 10d    # Add 10 days
datecalc 3600s  # Add 3600 seconds (1 hour)
```

    You can combine options to customize the output:
        -a: Show both dates (starting and ending date)
        -t: Show the time taken for calculations.
        -y: Use year zero.
        -k: Include the weekday in the output (first 3 letters).
        -f: Include the weekday (full name)
        -b: Don't use the unicode box

Example

```bash
./datecalc -ab 1000d
```

This command will calculate the date that is 1000 days after today and will not use unicode boxe for date output