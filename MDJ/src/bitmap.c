#include "bitmap.h"

int cantidadBloquesBitmapEnBytes(){
	int total = CANTIDAD_BLOQUES/8;
	if(CANTIDAD_BLOQUES % 8 > 0){
		total++;
	}
	return total;
}

t_bitarray* crearBitarray(){
	char* byteArray = asignarMemoria(cantidadBloquesBitmapEnBytes());

	FILE* archivoBitMap = fopen(rutaBitmap, "rb");
	fread(byteArray, cantidadBloquesBitmapEnBytes(), 1, archivoBitMap);

	return bitarray_create_with_mode(byteArray, cantidadBloquesBitmapEnBytes(), MSB_FIRST);
}

void persistirBitMap(){
	FILE* archivoBitMap = fopen(rutaBitmap, "wb");  // w for write, b for binary

	fwrite(bitarray->bitarray, cantidadBloquesBitmapEnBytes(), 1, archivoBitMap);

	fclose(archivoBitMap);
}

void initBitmap(){
	rutaBitmap = concatenar(PUNTO_MONTAJE_METADATA, "Bitmap.bin");
	bitarray = crearBitarray();
}

int cantidadTotalDeBloquesLibres(){
	int cantidadLibre = 0;
	for(int i = 0; i < bitarray_get_max_bit(bitarray); i++){
		if(bitarray_test_bit(bitarray, i) == 0){
			cantidadLibre++;
		}
	}
	return cantidadLibre;
}
int ocuparBloqueEnBitmap(int bloqueAOcupar){
	bitarray_set_bit(bitarray, bloqueAOcupar);
	persistirBitMap();
	return 0;
}

int liberarBloqueEnBitmap(int bloqueALiberar){
	bitarray_clean_bit(bitarray, bloqueALiberar);
	persistirBitMap();
	return 0;
}

int obtenerBloqueLibreBitmap(){
	int posicion;
	for(int i = 0; i < bitarray_get_max_bit(bitarray); i++){
		if(bitarray_test_bit(bitarray, i) == 0){
			posicion = i;
			break;
		}
	}
	return posicion;
}






