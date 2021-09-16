/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
extern int run_compact_test();
extern int run_fixed_int_test();

int main() {
  printf("\nRunning Fixed Int Tests\n");
  run_fixed_int_test();
  printf("\nRunning Compact Tests\n");
  run_compact_test();
  printf("\nDone\n");
  return 0;
}
