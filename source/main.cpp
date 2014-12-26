#include "Constants.h"

#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
   printf("%s %s %d.%d.%d.%d\n", PROJECT_NAME, VERSION_TYPE, VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, VERSION_BUILD);
   return EXIT_SUCCESS;
}
