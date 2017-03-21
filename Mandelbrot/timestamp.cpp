#include <time.h>
#include <time.h>
#include "timestamp.h"

void timestamp(char* dest) {
	time_t rawtime;
	struct tm* t;

	time(&rawtime);
	t = localtime(&rawtime);
	strftime(dest, 13, "[%X]", t);
}
