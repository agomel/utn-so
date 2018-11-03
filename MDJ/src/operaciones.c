#include "operaciones.h"
char* obtenerDatosDeArchivo(int offset, int tamanio, char* path){
	size_t tamanioALeer = tamanio;
	off_t of = offset;
	int myFile = open(path, O_WRONLY | O_RDONLY | O_CREAT, S_IRWXU);
	if(myFile < 0){
		log_info(logger, "Error gato!");
	}

	/*struct stat myStat;
	if(fstat(myFile, &myStat)){
		//fstat error
	}

	off_t size = myStat.st_size;*/

	char* buffer = mmap(NULL, tamanioALeer, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, myFile, of);
	//free(rutaArchivo);
	return buffer;
}

int validarArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	log_info(logger, "Validando archivo en ruta: %s", rutaArchivo);
	FILE* archivo = fopen(rutaArchivo, "r");
	if(archivo == NULL){
		return PATH_INEXISTENTE;
	}
	//0 es archivo valido
	return 0;
}

int crearArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	int cantidadDeBytes = deserializarInt(emisor);
	log_info(logger, "Creando archivo en ruta: %s", rutaArchivo);
	//TODO verificar si hay espacio antes de crearlo y esa movida
	//TODO se tiene que crear con esa cantidad de bytes.. debe ser para el bitmap
	/*FILE* archivo = fopen(rutaArchivo, "w");
	if(archivo == NULL){
		return ESPACIO_INSUFICIENTE;// nose que error podria pasar aca
	}
	*/

	char* buffer = obtenerDatosDeArchivo(0, cantidadDeBytes, rutaArchivo);
	for(int i = 0; i<cantidadDeBytes; i++){
		buffer[i] = '/0';
	}

	return 0;
}

void guardarDatos(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	int offset = deserializarInt(emisor);
	int tamanioALeer = deserializarInt(emisor);
	char* datos = deserializarString(emisor);
	//log_info("Guardando datos: %s en archivo: %s", datos, rutaArchivo);

	char* buffer = obtenerDatosDeArchivo(offset, tamanioALeer, rutaArchivo);
	buffer[0] = datos;

	log_info(logger, "Guardados los datos %s", datos);
	free(rutaArchivo);
	free(datos);


	//TODO guardar datos posta y hacer free de los strings
}

char* obtenerDatos(int emisor){
	char* rutaArchivo = "a";//deserializarString(emisor);
	int offset = 10;//deserializarInt(emisor);
	int tamanioALeer = 5;//deserializarInt(emisor);

	size_t tamanio = tamanioALeer;
	off_t of = offset;
	int myFile = open(rutaArchivo, O_WRONLY | O_RDONLY | O_CREAT, S_IRWXU);
	if(myFile < 0){
		log_info(logger, "Error gato!");
	}

	/*struct stat myStat;
	if(fstat(myFile, &myStat)){
		//fstat error
	}

	off_t size = myStat.st_size;*/

	char* buffer = mmap(NULL, tamanio, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, myFile, of);
	//free(rutaArchivo);
	return buffer;
 }

int eliminarArchivo(int emisor){
	char* rutaArchivo = deserializarString(emisor);
	if(remove(rutaArchivo) == 0){
		return 0;
	}else{
		//No se pudo eliminar el archivo
		return PATH_INEXISTENTE;
	}
}

