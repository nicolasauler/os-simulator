#include "logger.h"

/* reset log file */
void log_init() {
    FILE *fp;
    fp = fopen("log.txt", "w");
    fclose(fp);
}

/* write log */
void logger(const char *message) {
    FILE *fp;
    fp = fopen("log.txt", "a");
    fprintf(fp, "%s\n", message);
    fclose(fp);
}
