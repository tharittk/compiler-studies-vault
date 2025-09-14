
#include <stdio.h>
int main() {
  int y;
  int z;
  int i = 5;

  // BB1 entry
  while (i > 0) {
    if (i > 1) {
      // BB2: 80% here
      y = 4;

    } else {
      // BB3: 20 % here
      y = 3;
    }

    // BB4
    if (y / 2 != 0) {
      // BB5: 10%
      z = 1;
    }
    // BB6
    printf("i:%d y=%d\n", i, y);
    --i;
  }
  return 0;
}