#include <string.h>             // strchr
#include <dspc/strip_path.h>


/* found in a jack.* utilities, or jackd1 distrib example */

void strip_path(char *path, char *stripped) {
    char *cindex;

    if (path != NULL || path != (void *) 0) {
        cindex = strrchr(path, '/');
        if (cindex == 0) {
            cindex = path;
        } else {
            cindex++;
        }
        strncpy(stripped, cindex, MAX_PATH_LEN);
    } else {
        strncpy(stripped, "unnamed", 8);
    }
}
