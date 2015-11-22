#!/bin/bash

echo "// File qmread.cpp " > paste.txt
echo "// ---8<--BEGIN---------------------------------------" >> paste.txt
cat qmread.cpp >> paste.txt
echo "// -----EOF------>8-----------------------------------" >> paste.txt

echo "// File lmfile.hpp " >> paste.txt
echo "// ---8<--BEGIN---------------------------------------" >> paste.txt
cat lmfile.hpp >> paste.txt
echo "// -----EOF------>8-----------------------------------" >> paste.txt

echo "// File lmfile.cpp " >> paste.txt
echo "// ---8<--BEGIN---------------------------------------" >> paste.txt
cat lmfile.cpp >> paste.txt
echo "// -----EOF------>8-----------------------------------" >> paste.txt

wgetpaste -l C++ paste.txt

rm paste.txt 
