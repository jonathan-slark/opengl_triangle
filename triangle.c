#include <stdlib.h>

#include "al.h"
#include "config.h"
#include "util.h"

int
main(void)
{
    al_createwindow(title, width, height);

    while (al_pollevents()) {
	al_drawframe();
    }

    return EXIT_SUCCESS;
}
