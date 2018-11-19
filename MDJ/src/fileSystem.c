#include "fileSystem.h"

int haySuficienteEspacio(int tamanioAGuardar){
	if((cantidadTotalDeBloquesLibres() * TAMANIO_BLOQUES) > tamanioAGuardar){
		return 0;
	}else{
		return ESPACIO_INSUFICIENTE_EN_MDJ;
	}
}

int crearArchivoFS(char* rutaArchivo, char* datosTotales){
	int error = haySuficienteEspacio(strlen(datosTotales));
	if(error == 0){
		t_list* bloques = crearArchivoEnBloques(datosTotales);
		char* texto = concatenar("TAMANIO=", intToString(strlen(datosTotales)));
		concatenarATexto(&texto, "\nBLOQUES=[");

		for(int i = 0; i< bloques->elements_count; i++){
			concatenarATexto(&texto, intToString(list_get(bloques, i)));

			if(i == bloques->elements_count -1){
				concatenarATexto(&texto, "]\n");
			}else{
				concatenarATexto(&texto, ",");
			}

		}
		error = crearArchivo(rutaArchivo);
		if(error == 0){
			error = guardarDatos(rutaArchivo, 0, strlen(texto), texto);
		}

		list_destroy(bloques);
		free(texto);
	}

	return error;
}

int borrarArchivoFS(char* rutaArchivo){
	int error = 0;
	Metadata* metaData = obtenerMetadata(rutaArchivo);
	int cantidadDeBloques = obtenerCantidadBloques(metaData->tamanio);
	for(int i = 0; i < cantidadDeBloques; i++){
		int bloque = atoi(metaData->bloques[i]);
		eliminarBloque(bloque);
	}
	error = eliminarArchivo(rutaArchivo);
	freeMetadata(metaData);
	return error;
}

char* obtenerDatosFS(char* rutaArchivo, int offset, int size){
	Metadata* metaData = obtenerMetadata(rutaArchivo);
	int posicionDelBloque = offset / TAMANIO_BLOQUES;
	int offsetEnBloque = offset % TAMANIO_BLOQUES;
	int sizeALeerEnBloque;


	int desplazamiento = 0;

	if(size == -1)
		size = metaData->tamanio;

	char* datosTotales = asignarMemoria(size + 1);
	datosTotales[size] = '\0';
	while(size > 0){
		if(size > (TAMANIO_BLOQUES - offsetEnBloque)){
			sizeALeerEnBloque = TAMANIO_BLOQUES - offsetEnBloque;
		}else{
			sizeALeerEnBloque = size;
		}

		int bloque = atoi(metaData->bloques[posicionDelBloque]);
		char * datos = obtenerDatosDeBloque(bloque, offsetEnBloque, sizeALeerEnBloque);
		memcpy(datosTotales + desplazamiento, datos, sizeALeerEnBloque);
		desplazamiento += sizeALeerEnBloque;

		free(datos);
		posicionDelBloque++;
		offsetEnBloque = 0;
		size = size - sizeALeerEnBloque;
	}
	freeMetadata(metaData);
	return datosTotales;
}

int guardarDatosFS(char* rutaArchivo, int offset, int size, char* datos){
	int error = 0;

	char* datosAGuardar = asignarMemoria(size);
	memcpy(datosAGuardar, datos, size);
	datosAGuardar[size] = '\0';

	Metadata* metaData = obtenerMetadata(rutaArchivo);
	char* datosRestantes = obtenerDatosFS(rutaArchivo, 0, metaData->tamanio);

	char* datosPrincipio = asignarMemoria(offset + 1);
	memcpy(datosPrincipio, datosRestantes, offset);
	datosPrincipio[offset] = '\0';

	char* datosFin = asignarMemoria((metaData->tamanio - offset) + 1);
	memcpy(datosFin, (datosRestantes + offset), (metaData->tamanio - offset));
	datosFin[(metaData->tamanio - offset)] = '\0';

	char* datosTotales = concatenar(datosPrincipio, datosAGuardar);
	concatenarATexto(&datosTotales, datosFin);

	borrarArchivoFS(rutaArchivo);
	error = crearArchivoFS(rutaArchivo, datosTotales);

	freeMetadata(metaData);
	free(datosAGuardar);
	free(datosRestantes);
	free(datosPrincipio);
	free(datosFin);
	free(datosTotales);

	return error;
}
















/*int guardarDatosFS(char* rutaArchivo, int offset, int size, char* datos){


	Metadata* metaData = obtenerMetadata(rutaArchivo);
	int posicionDelBloque = offset / TAMANIO_BLOQUES;
	int offsetEnBloque = offset % TAMANIO_BLOQUES;

	//obtengo los datos restantes y se los concateno a los nuevos
	int sizeRestante = metaData->tamanio - offset;
	char* datosRestantes = obtenerDatosFS(rutaArchivo, offset, sizeRestante);
	char* datosTotales = concatenar(datos, datosRestantes);
	int sizeTotal = strlen(datosTotales);

	int cantidadDeBloques = obtenerCantidadBloques(metaData->tamanio);
	int desde = atoi(metaData->bloques[posicionDelBloque]);

	int cantidadEscrita = 0;

	int sizeAEscribirEnBloque;
	if(size > (TAMANIO_BLOQUES - offsetEnBloque)){
		sizeAEscribirEnBloque = TAMANIO_BLOQUES - offsetEnBloque;
	}else{
		sizeAEscribirEnBloque = size;
	}
	guardarDatosEnBloque(1, offsetEnBloque, sizeAEscribirEnBloque, (datosTotales + cantidadEscrita));

	sizeTotal -= sizeAEscribirEnBloque;
	cantidadEscrita += sizeAEscribirEnBloque;
	offsetEnBloque = 0;



	for(int i = desde; i < cantidadDeBloques; i++){
		int bloque = atoi(metaData->bloques[i]);
		guardarDatosEnBloque(bloque, )

	}

	free(datosRestantes);
	free(datosTotales);

	return 0;
}*/
