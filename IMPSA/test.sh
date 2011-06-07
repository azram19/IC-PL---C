#!/bin/sh
echo "$(tput setaf 5)Compiling....$(tput sgr0)"
gcc -Wall -g -O2 -o assemble assemble.c
if gcc -q -o assemble assemble.c && echo OK ; then
  echo "$(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "$(tput setaf 1)[FAIL]$(tput sgr0)"
  exit;
fi
echo "$(tput setaf 5)Testing memory leaks....$(tput sgr0)"
valgrind --tool=memcheck --leak-check=yes ./assemble simple.s simple.ooout
valgrind --tool=memcheck --leak-check=yes ./assemble factorial.s factorial.ooout
valgrind --tool=memcheck --leak-check=yes ./assemble matmult.s matmult.ooout
echo "$(tput setaf 5)Assembling....$(tput sgr0)"
./assemble simple.s simple.ooout
./assemble factorial.s factorial.ooout
./assemble matmult.s matmult.ooout
echo "$(tput setaf 5)Testing results...$(tput sgr0)"

echo simple:
if diff simple.oout simple.ooout >/dev/null ; then
  echo "$(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "$(tput setaf 1)[FAIL]$(tput sgr0)"
fi

echo factorial:
if diff factorial.oout factorial.ooout >/dev/null ; then
  echo "$(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "$(tput setaf 1)[FAIL]$(tput sgr0)"
fi

echo matmult:
if diff matmult.oout matmult.ooout >/dev/null ; then
  echo "$(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "$(tput setaf 1)[FAIL]$(tput sgr0)"
echo factorial:
fi

