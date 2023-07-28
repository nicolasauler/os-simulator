#include "logger.h"

void logger(const char *message) {
    FILE *fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "%s\n", message);
    fclose(fp);
}
