#!/bin/bash
#day.bash

declare -i month
declare -i day
declare -i date=0	#var for caculating day of the date 

daysOfMonth=(0 31 28 31 30 31 30 31 31 30 31 30 31)
days=(Thursday Friday Saturday Sunday Monday Tuesday Wednesday)	

echo "========================================"
echo "December 31st on 2020 is Saturday"
echo -n "input month (1~12) : "
read month
echo -n "input day (1~31) : "
read day

for ((i=1;i<$month;i++))	#add days of the month
do
	date=$date+${daysOfMonth[i]}
done

date=($date+$day)%7

echo "2021/$month/$day is ${days[$date]}."
echo "========================================"
