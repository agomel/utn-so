#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <unistd.h>
#include <commons/string.h>
#include "FM9.h"
#include "segmentacionPura.h"
#include "segmentacionPag.h"
#include "paginasInvertidas.h"
#include <biblioteca/dtb.h>
#include <biblioteca/traductor.h>
#include <readline/readline.h>

void consola();
void dumpSegPag(int idDTB);
void dumpInvertida(int idDTB);
#endif /*CONSOLA_H_*/
