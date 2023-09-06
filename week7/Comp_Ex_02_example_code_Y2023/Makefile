# a simple Makefile for some C programs to experiment with 
# Concurrent processes, pipes and Rate-Monotonic Scheduling
# in a Linux or MacOS environment
#
# see:
# https://www.gnu.org/software/make/manual/make.html
# for documentation
#
# simple usage:
# make all

fork_and_shell_03:  fork_and_shell_03.c
	gcc -Werror -Wall -Wextra -o fork_and_shell_03 fork_and_shell_03.c

concurrent_sum_03: concurrent_sum_03.c
	gcc -Werror -Wall -Wextra -o concurrent_sum_03 concurrent_sum_03.c

RM_simulator_07: RM_simulator_07.c
	gcc -Werror -Wall -Wextra -o RM_simulator_07 RM_simulator_07.c

all:	fork_and_shell_03 concurrent_sum_03 RM_simulator_07