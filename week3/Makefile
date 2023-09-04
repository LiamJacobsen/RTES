# makefile for Workshop 03 for RTES, S2 Y2023
# By Andrew A.
#
# a simple Makefile for some C programs to experiment with time and clocks
# in a Linux or MacOS environment
#
# see:
# https://www.gnu.org/software/make/manual/make.html
# for documentation
#
# simple usage:
# make all

# Remind ourselves of the first workshop, how to use a simple circular buffer as a First In First Out (FIFO) queue
circular_queue_example_04:	circular_queue_example_04.c
	gcc -Wall -Wextra -Werror -o circular_queue_example_04 circular_queue_example_04.c 

# Remind ourselves of the first two workshops, how to adapt an existing queuing system to accomodate data from a different source, and type
linked_list_collatz_application_checked:	linked_list_collatz_application_checked.c
	gcc -Wall -Wextra -Werror -o linked_list_collatz_application_checked linked_list_collatz_application_checked.c


# Remind ourselves of the second workshop, how to measure time, in nanoseconds, in a UNIX-like system
timer_test_10:	timer_test_10.c
	gcc -std=gnu99 -Wall -Wextra -Werror -o timer_test_10 timer_test_10.c

# remind ourselves of workshop number two, can we generate pseudo random integer type numbers, to serve as data
library_random_03:	library_random_03.c
	gcc -Wall -Wextra -Werror -o library_random_03 library_random_03.c

# Six Easy Problems, for Workshop 03
# In a nutshell, can we capture time stamps and data at required time times, and store the data in a queue for later processing?  
sample_time_stamp_and_queue_workshop_03_problems:	sample_time_stamp_and_queue_workshop_03_problems.c
	gcc -std=gnu99 -Wall -Wextra -Werror -o sample_time_stamp_and_queue_workshop_03_problems sample_time_stamp_and_queue_workshop_03_problems.c

# one command to bind them all
all:	circular_queue_example_04 linked_list_collatz_application_checked timer_test_10 library_random_03 sample_time_stamp_and_queue_workshop_03_problems
