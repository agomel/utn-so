#ifndef COMANDOS_H_
#define COMANDOS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <unistd.h>
#include <openssl/md5.h>

extern char* MONTAJE_ACTUAL;

extern int CANTIDAD_BLOQUES;

void ls(char* parametro);
void cd(char* parametro);
void cat(char* parametro);
void md5(char* parametro);

#endif /*COMANDOS_H_*/
