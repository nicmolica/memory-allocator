#include <stdio.h>
#include <unistd.h>

main() {
  printf("%d\n", sysconf(_SC_PAGE_SIZE));
}
