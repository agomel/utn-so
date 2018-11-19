#include "pruebas.h"
void crearArchivitos(){
	for(int i = 0; i<7; i++){
		char* rutaArchivito= concatenar("PruebaBOAF", intToString(i));
		concatenarATexto(&rutaArchivito,".bin");
		char* completa = generarPathAbsoluto(rutaArchivito);
		int archivoExiste = validarArchivo(completa);
		if(archivoExiste != 0){
			crearArchivoFS(completa, "concentrar\nconcentrar\nconcentrar\nconcentrar"
					"\nconcentrar\nconcentrar\nconcentrar\nconcentrar\nconcentrar"
					"\nconcentrar\nconcentrar\nconcentrar\n");
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

	crearArchivoGral("pruebaGralBOAF1.bin", "abrir PruebaBOAF0.bin\nabrir PruebaBOAF1.bin\nabrir PruebaBOAF2.bin\nabrir PruebaBOAF3.bin\nabrir PruebaBOAF4.bin\nabrir PruebaBOAF5.bin\nabrir PruebaBOAF6.bin\n");

	crearArchivoGral("pruebaGralBOAF2.bin", "abrir PruebaBOAF0.bin\nabrir PruebaBOAF1.bin\n");

}
