#!/bin/bash


usage () {
	echo
	echo "usage: $0 infile outfile"
	echo
	echo "infile must be in .hex format - rrggbb in hex, one per line"
	echo
	exit 1
}

[ 
