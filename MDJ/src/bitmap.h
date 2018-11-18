#ifndef BITMAP_H_
#define BITMAP_H_


#include "comandos.h"
#include "operacionesArchivos.h"
#include <commons/bitarray.h>

void verificarExistenciaDeBitmap();
int obtenerBloqueLibreBitmap();
int ocuparBloqueEnBitmap(int bloqueAOcupar);
int liberarBloqueEnBitmap(int bloqueALiberar);
void initBitmap();
int cantidadTotalDeBloquesLibres();

char* rutaBitmap;
t_bitarray* bitarray;

extern t_log* logger;
extern char* PUNTO_MONTAJE_METADATA;

extern char* MAGIC_NUMBER;
#endif /*BITMAP_H_*/
