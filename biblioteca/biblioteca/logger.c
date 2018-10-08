#include "logger.h"

t_log* crearLogger(char* file, char *programName){
	return log_create(file, programName, 1, LOG_LEVEL_DEBUG);
}

