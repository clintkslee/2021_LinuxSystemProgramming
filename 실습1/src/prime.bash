#!/bin/bash
#prime.bash

declare -i a
declare -i b
declare -i sum=0
declare -i count=0
echo "========================================"
echo -n "input a (a>=1) : "
read a
echo -n "input b (b>=a) : "
read b

while (($a<=$b))
do
	if (($a==1))		  # 1 is not PRIME
	then
		let a++
		continue
	fi
	
	for ((i=2;i<=$a;i++))
	do
		if (($i==$a))     # PRIME
		then
			sum=$sum+$a
			let count++
		elif (($a%$i==0)) # NOT PRIME
		then
			break
		fi
	done
	
	let a++				  
done

echo "There are $count PRIME numbers between a and b"
echo -n "Sum of PRIME numbers between a and b : "
echo $sum
echo "========================================"
