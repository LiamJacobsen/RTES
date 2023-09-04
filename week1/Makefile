# a simple Makefile for some C programs to experiment with some rudiments of C programming
# in a UNIX-like environment, such as BSD UNIX, macOS or Linux
#
# Last edit: Tue 18 Jul 2023 22:29:14 (Singapore)
#
# tested under macOS
# > uname -a
# Darwin Andrews-MacBook-Air.local 19.6.0 Darwin Kernel Version 19.6.0: Tue Jun 21 21:18:39 PDT 2022; 
# root:xnu-6153.141.66~1/RELEASE_X86_64 x86_64
#
# see:
# https://www.gnu.org/software/make/manual/make.html
# for documentation
#
# simple usage:
# make all

circular_queue_example_04:	circular_queue_example_04.c
	gcc -Wall -Wextra -Werror -o circular_queue_example_04 circular_queue_example_04.c 

collatz_01:	collatz_01.c
	gcc  -Wall -Wextra -Werror -o collatz_01 collatz_01.c

linked_list_example_04:	linked_list_example_04.c
	gcc  -Wall -Wextra -Werror -o linked_list_example_04 linked_list_example_04.c

sequential_sieve_06:	sequential_sieve_06.c
	gcc  -Wall -Wextra -Werror -o sequential_sieve_06 sequential_sieve_06.c

stdio_test_01:	stdio_test_01.c
	gcc -Wall -Wextra -Werror -o stdio_test_01 stdio_test_01.c

all:	circular_queue_example_04 collatz_01 linked_list_example_04 sequential_sieve_06 stdio_test_01
