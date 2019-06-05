#!/bin/bash

Compare() {
	#cmp --silent a b || (echo "DIFFERENT FILES!" && exit)
	if cmp --silent a b
	then
	echo "OK"
	else
	echo "DIFFERENT FILES!"
	sleep 7
	exit
	fi
}

do_test() {
	echo "TEST: '$3'"
	for((i = 0; i < $1; i++))
	do
		./ProjectCPP $2 a b
		echo "test-$3 $i"
		Compare
	done
	echo "PASSED: '$3' tests"
	sleep 4
}

do_test 1 0 "0file"
do_test 20 "--mode=debug" "debug"
do_test 10 12000 "d12k"
do_test 10 64000 "d64k"
do_test 10 1024000 "d1m"

echo "ALL Test Passed"
sleep 10