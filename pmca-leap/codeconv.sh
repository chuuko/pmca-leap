#!/bin/bash

src='UTF-8'
trg='CP932'


cp -f ./Makefile_win ./Makefile
rm -f ./Makefile_win

mkdir ./tmp

for x in 'PMCA_PyMod.c' 'PMCA_SDLMod.c' 'PMCA_view.c' 'mlib_PMD_edit01.c' 'mlib_PMD_rw01.c' 'PMCA.h' 'mPMD.h'
do
	echo $x
	iconv -c -f $src -t $trg <./$x> ./tmp/$x
	cp -f ./tmp/$x ./$x
done

rm -fr ./tmp
