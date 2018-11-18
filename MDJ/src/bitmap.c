#include "bitmap.h"

int cantidadBloquesBitmapEnBytes(){
	int total = CANTIDAD_BLOQUES/8;
	if(CANTIDAD_BLOQUES % 8 > 0){
		total++;
	}
	return total;
}

t_bitarray* crearBitarray(char* bytearray){
	return bitarray_create_with_mode(bytearray, cantidadBloquesBitmapEnBytes(), LSB_FIRST);
}

t_bitarray* leerBitmap(){
	unsigned char byteArray[cantidadBloquesBitmapEnBytes() + 1];
	FILE *ptr;

	ptr = fopen(rutaBitmap,"rb");  // r for read, b for binary

	fread(byteArray,sizeof(byteArray),1,ptr);
	agregarBarraCero(byteArray);

	return crearBitarray(byteArray);
}

void escribirBitmap(char* bits){
	FILE *write_ptr;

	write_ptr = fopen(rutaBitmap,"wb");  // w for write, b for binary

	fwrite(bits,sizeof(bits),1,write_ptr);
}

int validarExistenciaDeBitmap(){
	FILE* archivo = fopen(rutaBitmap, "rb");
	if(archivo == NULL || archivo < 0){
		return PATH_INEXISTENTE;
	}
	fclose(archivo);
	return 0;
}


void verificarExistenciaDeBitmap(){

	int archivoExiste = validarExistenciaDeBitmap();
	if(archivoExiste != 0){
		//El bitmap no existe
		char* datos = asignarMemoria(CANTIDAD_BLOQUES) + 1;
		for(int i = 0; i<CANTIDAD_BLOQUES; i++){
			datos[i] = '0';
		}
		//TODO esto esta mal esta escribiendo todo el bitmap

		escribirBitmap(datos);
		log_info(logger, "creado bitmap para %d bloques", CANTIDAD_BLOQUES);
	}
}

int obtenerBloqueLibreBitmap(){
	t_bitarray* bitmap = leerBitmap();
	int posicion;
	for(int i = 0; i < bitmap->size; i++){
		if(!bitarray_test_bit(bitmap, i)){
			posicion = i;
			break;
		}
	}
	bitarray_destroy(bitmap);
	return posicion;
}


int ocuparBloqueEnBitmap(int bloqueAOcupar){
	t_bitarray* bitmap = leerBitmap();
	bitarray_set_bit(bitmap, bloqueAOcupar);
	escribirBitmap(bitmap->bitarray);
	bitarray_destroy(bitmap);
}
int liberarBloqueEnBitmap(int bloqueALiberar){
	t_bitarray* bitmap = leerBitmap();
	bitarray_clean_bit(bitmap, bloqueALiberar);
	escribirBitmap(bitmap->bitarray);
	bitarray_destroy(bitmap);
}

void initBitmap(){
	rutaBitmap =concatenar(PUNTO_MONTAJE_METADATA, "Bitmap.bin");
}
