#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
    exit(1);
  }
  int n = atoi(argv[1]);
  int i = 0, sum = 0;
  for (i = 0; i < n; i++)
    sum += i;

  printf("sum = %d\n", sum);
  return 0;
}
