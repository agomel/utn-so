#ifndef COMANDOS_H_
#define COMANDOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <unistd.h>

extern char* PUNTO_MONTAJE;

void ls(char* parametro);
void cd(char* parametro);
void cat(char* parametro);

#endif /*COMANDOS_H_*/
