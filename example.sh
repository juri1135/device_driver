#!/bin/sh

list="2 3 4 5 6 7 8 9"

	if [ $1 -ge 2 ] && [ $1 -le 9 ];
		then for var in $list
			do echo "$1 * $var = $(($var * $1))"
			done
	else
		echo "wrong input"
	fi


