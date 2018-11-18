#include "bitmap.h"

int cantidadBloquesBitmapEnBytes(){
	int total = CANTIDAD_BLOQUES/8;
	if(CANTIDAD_BLOQUES % 8 > 0){
		total++;
	}
	return total;
}

void crearBitarray(char* bytearray){
	bitmap = bitarray_create_with_mode(bytearray, cantidadBloquesBitmapEnBytes(), LSB_FIRST);
}


char* crearMmap(int archivo){
	int bloques = cantidadBloquesBitmapEnBytes();
	char* byteArray = asignarMemoria(bloques);
	void* mapita = mmap(NULL, bloques, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, archivo, 0);
	  if (mapita == MAP_FAILED) {
	    perror("Could not mmap");
	    perror(strerror(errno));
	    return 1;
	  }

	  byteArray = mapita;
	 return byteArray;
}
void leerBitmap(){
	FILE *ptr;

	ptr = fopen(rutaBitmap,"r+b");  // r for read, b for binary

	char* byteArrayMapeado = crearMmap(ptr);
	crearBitarray(byteArrayMapeado);
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
	int posicion;
	for(int i = 0; i < bitmap->size; i++){
		if(!bitarray_test_bit(bitmap, i)){
			posicion = i;
			break;
		}
	}
	return posicion;
}


int ocuparBloqueEnBitmap(int bloqueAOcupar){
	bitarray_set_bit(bitmap, bloqueAOcupar);
}
int liberarBloqueEnBitmap(int bloqueALiberar){
	bitarray_clean_bit(bitmap, bloqueALiberar);
}

void initBitmap(){
	rutaBitmap =concatenar(PUNTO_MONTAJE_METADATA, "Bitmap.bin");
	leerBitmap();
}
