#include "../include/lwp.h"

int main(void) {
    int i;
    for (i = 0; i < 5; i++)
        lwp_create(NULL, 0);

    return 0;
}
