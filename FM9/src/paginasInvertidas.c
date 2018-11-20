#include "paginasInvertidas.h"

static void cargarTabla();
static int obtenerMarcoLibreInvertida();
static ElementoTablaInvertida* obtenerElementoPorIdDTB(int idDTB);
static ElementoTablaInvertida* obtenerElementoPorMarco(int marco);
static t_list* filtrarPorDTBYArchivo(int idDTB, char* nombreArchivo);

void inicializarInvertida(t_config* configuracion){
	tablaPaginasInvertidas = list_create();
	idPagina = 0;
	idMarco = 0;
	tamanioPagina = config_get_int_value(configuracion, "TAM_PAGINA");
	cantidadMarcosTotales = tamanioMemoria / tamanioPagina;
	cargarTabla();
}

static void freeLineasBasura(char** lineaSinBasura, char* lineaConBasura){
	free(lineaConBasura);
	free(lineaSinBasura[0]);
	free(lineaSinBasura[1]);
	free(lineaSinBasura);
}

respuestaDeObtencionDeMemoria* obtenerDatosInvertida(int idDTB, char* nombreArchivo){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	t_list* marcosConEseArchivo = filtrarPorDTBYArchivo(idDTB, nombreArchivo);

		char* archivo = string_new();
		for(int i=0; i < marcosConEseArchivo->elements_count; i++){
			ElementoTablaInvertida* pagina = list_get(marcosConEseArchivo, i);

			int desplazamiento = storage + pagina->marco * (tamanioPagina * tamanioLinea); //porque el tamanio de pag esta en lineas
			int numeroLinea = 0;
			int finPagina = 0;

			while(numeroLinea < tamanioPagina && !finPagina){
				char* lineaConBasura = malloc(tamanioLinea);
				memcpy(lineaConBasura, desplazamiento + numeroLinea * tamanioLinea, tamanioLinea);
				if(lineaConBasura[0] == '\n'){ //Ultima linea archivo
					finPagina = 1;
					string_append(&archivo, "\n");
					free(lineaConBasura);
				}else{
					char** lineaSinBasura = string_split(lineaConBasura, "\n");
					string_append_with_format(&archivo, "%s\n", lineaSinBasura[0]);
					freeLineasBasura(lineaSinBasura, lineaConBasura);
				}
			}
		}
		respuesta->datos = malloc(strlen(archivo) + 1);
		memcpy(respuesta->datos, archivo, strlen(archivo) + 1);
		return respuesta;
}

static t_list* filtrarPorDTBYArchivo(int idDTB, char* nombreArchivo){
	bool coincidenIdyArchivo(ElementoTablaInvertida* elemento){
		return elemento->idDTB == idDTB && (strcmp(elemento->nombreArchivo, nombreArchivo) == 0);
	}

	bool compararPorPagina(ElementoTablaInvertida* elem1, ElementoTablaInvertida* elem2){
		return elem1->pagina < elem2->pagina;
	}

	t_list* lista = list_filter(tablaPaginasInvertidas, coincidenIdyArchivo);
	list_sort(lista, compararPorPagina);
	return lista;
}

respuestaDeObtencionDeMemoria* obtenerLineaInvertida(int idDTB, char* nombreArchivo, int numeroLinea){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	t_list* marcosConEseArchivo = filtrarPorDTBYArchivo(idDTB, nombreArchivo);

	int paginaDondeSeEncuentraLaLinea = numeroLinea / tamanioPagina;
	int lineaDentroDeLaPagina = numeroLinea % tamanioPagina;
	ElementoTablaInvertida* pagina = list_get(marcosConEseArchivo, paginaDondeSeEncuentraLaLinea);
	int desplazamientoPagina = pagina->marco * tamanioPagina * tamanioLinea; //Porque el tamanioPagina esta en lineas
	int desplazamientoLinea = lineaDentroDeLaPagina * tamanioLinea;
	char* lineaConBasura = asignarMemoria(tamanioLinea);
	memcpy(lineaConBasura, storage + desplazamientoPagina + desplazamientoLinea, tamanioLinea);
	if(lineaConBasura[0]=='\n'){ //FIN DE ARCHIVO
		respuesta->pudoObtener = 3;
		free(lineaConBasura);
	}else{
		log_debug(logger, "Linea completa: %s", lineaConBasura);
		char** lineaSinBasura = string_split(lineaConBasura, "\n");
		log_debug(logger, "Linea limpia: %s", lineaSinBasura[0]);
		respuesta->datos = string_new();
		respuesta->pudoObtener = 0;
		string_append(&respuesta->datos, lineaSinBasura[0]);
		freeLineasBasura(lineaSinBasura, lineaConBasura);
	}

	list_destroy(marcosConEseArchivo);
	return respuesta;
}

static void cargarElemento(ElementoTablaInvertida* elemento, int pagina, char* nombreArchivo, int idDTB){
	elemento->pagina = pagina;
	elemento->idDTB = idDTB;
	elemento->nombreArchivo = malloc(strlen(nombreArchivo) + 1);
	memcpy(elemento->nombreArchivo, nombreArchivo, strlen(nombreArchivo) + 1);
}

RespuestaGuardado* guardarDatosInvertida(int idDTB, char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en paginas invertidas");

	RespuestaGuardado* respuesta = malloc(sizeof(RespuestaCargaInvertida));
	int totalLineas = cantidadDeLineas(datos);
	char** lineas = string_split(datos, "\n");
	int cantidadPaginas = 1;
	int lineasEnLaUltimaPagina = totalLineas % tamanioPagina;
	if(totalLineas > tamanioPagina){
		cantidadPaginas = totalLineas / tamanioPagina;
		if(lineasEnLaUltimaPagina != 0)
				cantidadPaginas++;
	}

	int lineaACargar = 0;
	for(int i = 0; i < cantidadPaginas; i++){
		char* textoAGuardar;
		int marcoAUtilizar = obtenerMarcoLibreInvertida();
		if(marcoAUtilizar == -1)
			respuesta->pudoGuardar = -1; //ERROR NO HAY MARCOS LIBRES
		else
			respuesta->pudoGuardar = 0; //No hay error
		ElementoTablaInvertida* elementoInvertida = obtenerElementoPorMarco(marcoAUtilizar);
		cargarElemento(elementoInvertida, idPagina, nombreArchivo, idDTB);
		idPagina++;
		int base = storage + marcoAUtilizar * (tamanioPagina * tamanioLinea); //Porque el tamanioPagina esta en lineas
		if(cantidadPaginas - 1 == i){ //Es la ultima pagina
			for (int j = 0;  j < lineasEnLaUltimaPagina; j++) {
				if(lineas[lineaACargar]==NULL){
					lineas[lineaACargar] = string_new();
					if(lineaACargar != (totalLineas - 1)){ //No es ultima linea
						lineas[lineaACargar + 1] = malloc(sizeof(char));
						lineas[lineaACargar + 1] = NULL;
					}
				}
				string_append(&lineas[lineaACargar], "\n");
				char* textoAEscribir = malloc(tamanioLinea);
				memcpy(textoAEscribir, lineas[lineaACargar], strlen(lineas[lineaACargar]) + 1);
				memcpy(base + tamanioLinea * j, textoAEscribir, tamanioLinea); //Guardando de a una linea
				free(textoAEscribir);
				lineaACargar++;
			}
		}else{
			for(int j = 0; j < (tamanioPagina); j++){
				if(lineas[lineaACargar]==NULL){
					lineas[lineaACargar] = string_new();
					if(lineaACargar != (totalLineas - 1)){ //No es ultima linea
						lineas[lineaACargar + 1] = malloc(sizeof(char));
						lineas[lineaACargar + 1] = NULL;
					}
				}
				string_append(&lineas[lineaACargar], "\n");
				char* textoAEscribir = malloc(tamanioLinea);
				memcpy(textoAEscribir, lineas[lineaACargar], strlen(lineas[lineaACargar]) + 1);
				memcpy(base + tamanioLinea * j, textoAEscribir, tamanioLinea); //Guardando de a una linea
				free(textoAEscribir);
				lineaACargar++;
			}
		}
	}
		log_debug(logger, "Datos guardados");
		respuesta->pesoArchivo = cantidadPaginas * tamanioPagina;

	freeLineas(lineas);
	return respuesta;
}

static ElementoTablaInvertida* obtenerElementoPorMarco(int marco){
	bool coincideMarco(ElementoTablaInvertida* elemento){
		return elemento->marco == marco;
	}

	return list_find(tablaPaginasInvertidas, coincideMarco);
}

static ElementoTablaInvertida* obtenerElementoPorIdDTB(int idDTB){
	bool coincideId(ElementoTablaInvertida* elemento){
		return elemento->idDTB == idDTB;
	}

	return list_find(tablaPaginasInvertidas, coincideId);
}

static int obtenerMarcoLibreInvertida(){
	bool marcoEstaLibre(ElementoTablaInvertida* elemento){
		return elemento->pagina == -1;
	}

	ElementoTablaInvertida* elemento = list_find(tablaPaginasInvertidas, marcoEstaLibre);
	if(elemento != NULL){
		return elemento->marco;
	}else{
		log_error(logger, "No quedan marcos disponibles");
		return -1;
	}
}

static ElementoTablaInvertida* crearElementoTablaInvertida(){
	ElementoTablaInvertida* elemento = malloc(sizeof(ElementoTablaInvertida));
	elemento->marco = idMarco;
	idMarco++;
	elemento->idDTB = NULL;
	elemento->nombreArchivo = NULL;
	elemento->pagina = -1;
	return elemento;
}

static void cargarTabla(){
	for(int i=0; i < cantidadMarcosTotales; i++){
		ElementoTablaInvertida* elemento = crearElementoTablaInvertida();
		list_add(tablaPaginasInvertidas, elemento);
	}
}
