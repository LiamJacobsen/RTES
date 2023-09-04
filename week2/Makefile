# a simple Makefile for some C programs to experiment with time and clocks
# in a Linux or MacOS environment
#
# see:
# https://www.gnu.org/software/make/manual/make.html
# for documentation
#
# simple usage:
# make all

linked_list_collatz_application_checked: linked_list_collatz_application_checked.c 
	gcc -Wall -Wextra -Werror -o linked_list_collatz_application_checked linked_list_collatz_application_checked.c

circular_queue_example_04:	circular_queue_example_04.c 
	gcc -Wall -Wextra -Werror -o circular_queue_example_04 circular_queue_example_04.c

timer_test_08:	timer_test_08.c
	gcc -std=gnu99 -Wall -Wextra -Werror -o timer_test_08 timer_test_08.c -lm

timer_test_09:	timer_test_09.c
	gcc -std=gnu99 -Wall -Wextra -Werror -o timer_test_09 timer_test_09.c -lm

library_random_03:	library_random_03.c 
	gcc -Wall -Wextra -Werror -o library_random_03 library_random_03.c

timer_test_10:	timer_test_10.c 
	gcc -std=gnu99 -Wall -Wextra -Werror -o timer_test_10 timer_test_10.c -lm 

sample_time_stamp_and_queue_workshop_01:	sample_time_stamp_and_queue_workshop_01.c 
	gcc -std=gnu99 -Wall -Wextra -Werror -o sample_time_stamp_and_queue_workshop_01 sample_time_stamp_and_queue_workshop_01.c

all:	circular_queue_example_04 library_random_03 linked_list_collatz_application_checked sample_time_stamp_and_queue_workshop_01 timer_test_08 timer_test_09 timer_test_10