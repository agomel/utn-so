#include "pruebas.h"
void crearArchivitos(){
	for(int i = 0; i < 5; i++){
		char* rutaArchivito= concatenar("PruebaBOAF", intToString(i));
		concatenarATexto(&rutaArchivito,".bin");
		char* completa = generarPathAbsoluto(rutaArchivito);
		int archivoExiste = validarArchivo(completa);
		if(archivoExiste != 0){
			crearArchivoFS(completa, "concentrar\nconcentrar\nconcentrar\n\n");
		}
		free(completa);
		free(rutaArchivito);
	}
}

void crearArchivoGral(char* rutaArchivo, char* datos){
	char* rutaCompleta = generarPathAbsoluto(rutaArchivo);
	int archivoExiste = validarArchivo(rutaCompleta);

	if(archivoExiste != 0){
		//El archivo de pruebas no existe
		crearArchivoFS(rutaCompleta, datos);
		log_info(logger, "creado archivo de pruebas para BOAF con nombre %s", rutaArchivo);
	}
	free(rutaCompleta);
}

void crearArchivoDePruebas(){
	crearArchivitos();

	crearArchivoGral("PGB1.bin", "abrir PruebaBOAF0.bin\nabrir PruebaBOAF1.bin\nabrir PruebaBOAF2.bin\nabrir PruebaBOAF3.bin\nabrir PruebaBOAF4.bin\n\n");

	crearArchivoGral("PGB2.bin", "abrir PruebaBOAF0.bin\nabrir PruebaBOAF1.bin\n\n");
}
