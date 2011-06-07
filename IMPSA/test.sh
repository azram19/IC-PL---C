#!/bin/sh
echo "$(tput setaf 5)Compiling....$(tput sgr0)"
if gcc -Wall -g -O2 -o assemble assemble.c ; then
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
rm simple.ooout
rm factorial.ooout
rm matmult.ooout
if ./assemble simple.s simple.ooout ; then 
  echo "Simple: $(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "Simple: $(tput setaf 1)[FAIL]$(tput sgr0)"
    exit;
fi
if ./assemble factorial.s factorial.ooout ; then 
  echo "Factorial: $(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "Factorial:  $(tput setaf 1)[FAIL]$(tput sgr0)"
    exit;
fi
if ./assemble matmult.s matmult.ooout ; then 
  echo "Matmult: $(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "Matmult: $(tput setaf 1)[FAIL]$(tput sgr0)"
  exit;
fi
echo "$(tput setaf 5)Testing results...$(tput sgr0)"

if diff simple.oout simple.ooout >/dev/null ; then
  echo "Simple: $(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "Simple: $(tput setaf 1)[FAIL]$(tput sgr0)"
fi

if diff factorial.oout factorial.ooout >/dev/null ; then
  echo "Factorial: $(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "Factorial:  $(tput setaf 1)[FAIL]$(tput sgr0)"
fi

if diff matmult.oout matmult.ooout >/dev/null ; then
  echo "Matmult: $(tput setaf 2)[OK]$(tput sgr0)"
else
  echo "Matmult: $(tput setaf 1)[FAIL]$(tput sgr0)"
echo factorial:
fi

