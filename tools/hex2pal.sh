#!/bin/bash

# stub tool - hard-wired to the task at hand. Will extend later.

usage () {
	echo
	echo "usage: $0 [-p] infile outfile"
cat << EOFOUT

  -p  Include PRG 2-byte header (dummy 0x0000 used)
      Default behavior is no 2-byte header.

infile:
      Must be in .hex format - rrggbb in hex, one per line

outfile:
      Overwritten w/o confirmation if it already exists

EOFOUT
	exit 1
}

hex2bin() {
	line=`echo $1|tr A-Z a-z|sed -e 's/[^a-z0-9]//g'`
	#[ `echo $line|wc -c` -ne 6 ] && continue
	r=`echo $1|cut -b 2`
	g=`echo $1|cut -b 4`
	b=`echo $1|cut -b 6`
	#lo="\\x$g$b"
	#hi="\\x0$r"
	echo -n -e "\\x$g$b" >> $outfile
	echo -n -e "\\x0$r" >> $outfile
	((count++))
}

HDR=0

[ $# -lt 2 ] && usage
if [ "$1" == "-p" ]
then
	HDR=1
	shift
fi
[ $# -ne 2 ] && usage

if [ ! -r "$1" ]
then
	echo "Error: Unable to open $1 for input."
	echo
	exit 1
fi

if [ "$1" == "$2" ]
then
	echo "Error: input and output files must be different."
	echo
	exit 1
fi

if [ -x "$2" ]
then
	if [ ! -w "$2" ]
	then
		echo "Error: cannot write output file $2."
		echo
		exit 1
	fi
else
	touch "$2"
	if [ $? -ne 0 ]
	then
		echo "Error: cannot write output file $2."
		echo
		exit 1
	else
		rm "$2"
	fi
fi

outfile=`basename "$2"`
outfile="/tmp/$outfile"

if [ $HDR -eq 1 ]
then
	echo -n -e "\x00\x00" > $outfile
else
	cat /dev/null > $outfile
fi

count=0
while read -r line ; do
	hex2bin $line
done < "$1"

if [ $count -gt "0" ]
then
	mv $outfile "$2"
	echo "$count palette entries written into $2"
else
	rm $outfile
	echo "no colors found."
fi
