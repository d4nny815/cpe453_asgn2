#include "../include/lwp.h"

int main(void) {
    for (int i = 0; i < 5; i++)
        lwp_create(NULL, 0);

    return 0;
}
