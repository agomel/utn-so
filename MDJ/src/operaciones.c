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


char* leerTodoElArchivo(char *filename)
{
   char *buffer = NULL;
   int string_size, read_size;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
       // Seek the last byte of the file
       fseek(handler, 0, SEEK_END);
       // Offset from the first to the last byte, or in other words, filesize
       string_size = ftell(handler);
       // go back to the start of the file
       rewind(handler);

       // Allocate a string that can hold it all
       buffer = (char*) malloc(sizeof(char) * (string_size + 1) );

       // Read it all in one operation
       read_size = fread(buffer, sizeof(char), string_size, handler);

       // fread doesn't set it so put a \0 in the last position
       // and buffer is now officially a string
       buffer[string_size] = '\0';

       if (string_size != read_size)
       {
    	   log_error(logger, "hubo un error al leer el archivo :(");
           // Something went wrong, throw away the memory and set
           // the buffer to NULL
           free(buffer);
           buffer = NULL;
       }

       // Always remember to close the file.
       fclose(handler);
    }

    return buffer;
}

char* leerParteDeArchivo(char *filename, int tamanioALeer, int offset)
{
   char *buffer = NULL;
   FILE *handler = fopen(filename, "r");

   if (handler)
   {
	   lseek(handler, offset, SEEK_SET);

       // Allocate a string that can hold it all
       buffer = (char*) malloc(sizeof(char) * (tamanioALeer + 1) );

       // Read it all in one operation
       int tamanioLeido = fread(buffer, sizeof(char), tamanioALeer, handler);

       // fread doesn't set it so put a \0 in the last position
       // and buffer is now officially a string
       buffer[tamanioALeer] = '\0';

       if (tamanioALeer != tamanioLeido)
       {
    	   log_error(logger, "hubo un error al leer el archivo de a partes :(");
           // Something went wrong, throw away the memory and set
           // the buffer to NULL
           free(buffer);
           buffer = NULL;
       }

       // Always remember to close the file.
       fclose(handler);
    }

    return buffer;
}
char* obtenerDatos(int emisor){
	char* rutaArchivo = "a";//deserializarString(emisor);
	int offset = 10;//deserializarInt(emisor);
	int tamanioALeer = 5;//deserializarInt(emisor);

	//return obtenerDatosDeArchivo(offset, tamanioALeer, rutaArchivo);

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

