#include <stdlib.h>

#include "al.h"
#include "config.h"
#include "util.h"

int
main(void)
{
    if (!al_createwindow(title, width, height))
	terminate("Failed to create window.\n");

    while (al_pollevents()) {
	/*al_drawframe();*/
    }

    return EXIT_SUCCESS;
}
