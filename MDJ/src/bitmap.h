#ifndef BITMAP_H_
#define BITMAP_H_


#include "comandos.h"
#include "operacionesArchivos.h"
#include <commons/bitarray.h>

t_bitarray* crearBitarray();
void persistirBitMap();
void initBitmap();
int cantidadTotalDeBloquesLibres();
int ocuparBloqueEnBitmap(int bloqueAOcupar);
int liberarBloqueEnBitmap(int bloqueALiberar);
int obtenerBloqueLibreBitmap();

char* rutaBitmap;
t_bitarray* bitarray;

extern t_log* logger;
extern char* PUNTO_MONTAJE_METADATA;

extern char* MAGIC_NUMBER;
#endif /*BITMAP_H_*/
