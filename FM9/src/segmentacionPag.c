#include "segmentacionPag.h"
#include "stdbool.h"

static RespuestaCargaSegPag* guardarDatosInternaSegPag(char* datos, char* nombreArchivo);
static int obtenerMarcoLibre();

void inicializarSegPag(t_config* configuracion){
	idSegmento = 0;
	tablaDePaginas = list_create();
	idPagina = 0;
	tablaDeProcesos = list_create();
	tamanioPagina = config_get_int_value(configuracion, "TAM_PAGINA");
	cantidadMarcosTotales = tamanioMemoria / tamanioPagina;
	inicializarMutex(&mutexListaPaginas);
	inicializarMutex(&mutexListaSegmentos);
	inicializarMutex(&mutexListaProcesos);
}

static ElementoTablaDTBS* obtenerProcesoPorIdDTB(int idDTB){
	bool coincideId(ElementoTablaDTBS* elemento){
		return elemento->idDTB == idDTB;
	}
	waitMutex(&mutexListaProcesos);
	ElementoTablaDTBS* proc =  list_find(tablaDeProcesos, coincideId);
	signalMutex(&mutexListaProcesos);
	return proc;
}

static ElementoTablaSegPag* obtenerSegmentoPorArchivo(char* nombreArchivo, t_list* tablaSegmentos){
	bool coincideNombre(ElementoTablaSegPag* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0)
			return true;

		return false;
	}
	waitMutex(&mutexListaSegmentos);
	ElementoTablaSegPag* seg = list_find(tablaSegmentos, coincideNombre);
	signalMutex(&mutexListaSegmentos);
	return seg;
}

bool compararPorMarco(ElementoTablaPag* elem1, ElementoTablaPag* elem2){
	return elem1->marco < elem2->marco;
}

static int obtenerMarcoLibre(){
	if(tablaDePaginas->elements_count >= 1){

		if(tablaDePaginas->elements_count > 1){
			waitMutex(&mutexListaPaginas);
			list_sort(tablaDePaginas, compararPorMarco);
			signalMutex(&mutexListaPaginas);}
		waitMutex(&mutexListaPaginas);
		ElementoTablaPag* primerElemento = list_get(tablaDePaginas, 0);
		signalMutex(&mutexListaPaginas);
		if(primerElemento->marco != 0)
			return 0;
		waitMutex(&mutexListaPaginas);
		for(int i = 0; i < tablaDePaginas->elements_count - 1; i++){
			ElementoTablaPag* elem1 = list_get(tablaDePaginas, i);
			ElementoTablaPag* elem2 = list_get(tablaDePaginas, i+1);

			int posicionInicial = elem1->marco + 1;
			int tamanioEntreAmbos = elem2->marco - posicionInicial;

			if(tamanioEntreAmbos != 0){
				//Entra en ese lugar
				return posicionInicial;
			}
		}

		ElementoTablaPag* ultimoElemento = list_get(tablaDePaginas, tablaDePaginas->elements_count -1);
		signalMutex(&mutexListaPaginas);
		int ultimoMarco = ultimoElemento->marco;
		if((ultimoMarco + 1) < cantidadMarcosTotales){
			return ultimoMarco + 1;
		}else{
			log_error(logger, "No hay marcos libres");
			return -1;
		}
	}
	return 0;
}

ElementoTablaSegPag* crearElemTablaSegPag(char* nombreArchivo, int cantLineas){
	ElementoTablaSegPag* elemento = malloc(sizeof(ElementoTablaSegPag));
	elemento->cantidadLineas = cantLineas;
	elemento->id = idSegmento;
	elemento->nombreArchivo = malloc(strlen(nombreArchivo) + 1);
	memcpy(elemento->nombreArchivo, nombreArchivo, strlen(nombreArchivo) + 1);
	elemento->paginas = list_create();
	idSegmento++;
	return elemento;
}

ElementoTablaDTBS* crearElemTablaDTBS(int idDTB, t_list* tablaSegmentos){
	ElementoTablaDTBS* elemento = malloc(sizeof(ElementoTablaDTBS));
	elemento->idDTB = idDTB;
	elemento->segmentos = tablaSegmentos;
	return elemento;
}

static void freeRespuestaCargaSegPag(RespuestaCargaSegPag* respuesta){
	free(respuesta->elementoTabla->nombreArchivo);
	free(respuesta->elementoTabla);
	free(respuesta);
}

static void copiarElemSegPag(ElementoTablaSegPag* de, ElementoTablaSegPag* hasta){
	int tamanioArchivo = strlen(de->nombreArchivo);
	hasta->nombreArchivo = malloc(tamanioArchivo + 1);
	memcpy(hasta->nombreArchivo, de->nombreArchivo, tamanioArchivo + 1);
	hasta->nombreArchivo[tamanioArchivo] = '\0';
	hasta->id = de->id;
	hasta->paginas = de->paginas;
	hasta->cantidadLineas = de->cantidadLineas;
}

RespuestaGuardado* nuevoProcesoSegPag(int idDTB, char* datos, char* nombreArchivo){
	RespuestaCargaSegPag* cargaEnMemoria = guardarDatosInternaSegPag(datos, nombreArchivo);
	RespuestaGuardado* respuesta = malloc(sizeof(RespuestaGuardado));

	if(cargaEnMemoria->resultado == 0){ //No rompio
		t_list* tablaSegmentos = list_create();
		ElementoTablaSegPag* nuevoRegistro = malloc(sizeof(ElementoTablaSegPag));
		copiarElemSegPag(cargaEnMemoria->elementoTabla, nuevoRegistro);
		waitMutex(&mutexListaSegmentos);
		list_add(tablaSegmentos, nuevoRegistro);
		ElementoTablaDTBS* elementoTablaDTBS = crearElemTablaDTBS(idDTB, tablaSegmentos);
		signalMutex(&mutexListaSegmentos);
		waitMutex(&mutexListaProcesos);
		list_add(tablaDeProcesos, elementoTablaDTBS);
		signalMutex(&mutexListaProcesos);
		log_info(logger, "Agregado proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = 0;
		respuesta->pesoArchivo = cargaEnMemoria->pesoArchivo;
		freeRespuestaCargaSegPag(cargaEnMemoria);
	}else{
		log_error(logger, "No se pudo agregar proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = cargaEnMemoria->resultado;
		log_error(logger, "No se puede escribir en la ultima linea del archivo.");
	}
	return respuesta;
}

RespuestaGuardado* guardarDatosSegPag(int idDTB, char* datos, char* nombreArchivo){
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	RespuestaCargaSegPag* cargaEnMemoria = guardarDatosInternaSegPag(datos, nombreArchivo);
	RespuestaGuardado* respuesta = malloc(sizeof(RespuestaGuardado));
	if(cargaEnMemoria->resultado == 0){
		ElementoTablaSegPag* nuevoRegistro = malloc(sizeof(ElementoTablaSegPag));
		copiarElemSegPag(cargaEnMemoria->elementoTabla, nuevoRegistro);
		waitMutex(&mutexListaSegmentos);
		list_add(proceso->segmentos, nuevoRegistro);
		signalMutex(&mutexListaSegmentos);
		respuesta->pudoGuardar = 0;
		respuesta->pesoArchivo = cargaEnMemoria->pesoArchivo;
		freeRespuestaCargaSegPag(cargaEnMemoria);
	}else{
		respuesta->pudoGuardar = cargaEnMemoria->resultado; //ERROR
	}
	return respuesta;
}

static RespuestaCargaSegPag* guardarDatosInternaSegPag(char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en segmentacion paginada");

	RespuestaCargaSegPag* respuesta = malloc(sizeof(RespuestaCargaSegPag));
	respuesta->resultado = 1;
	int totalLineas = cantidadDeLineas(datos);
	char** lineas = dividirPorLineas(datos);
	int tamanioSegmento = totalLineas * tamanioLinea;
	int cantidadPaginas = 1;
	int lineasEnLaUltimaPagina = totalLineas % tamanioPagina;
	if (totalLineas == tamanioPagina) lineasEnLaUltimaPagina = tamanioPagina;
	if(totalLineas > tamanioPagina){
		cantidadPaginas = totalLineas / tamanioPagina;
		if(lineasEnLaUltimaPagina != 0)
			cantidadPaginas++;
	}
	if(lineasEnLaUltimaPagina == 0)
		lineasEnLaUltimaPagina = tamanioPagina - 1;

	if((cantidadMarcosTotales - tablaDePaginas->elements_count) >= cantidadPaginas){
		respuesta->resultado = 0; //No hay error
		ElementoTablaSegPag* elementoSegmento = crearElemTablaSegPag(nombreArchivo, totalLineas);
		int lineaACargar = 0;
		for(int i = 0; i < cantidadPaginas; i++){
			char* textoAGuardar;
			int posicionMarco = obtenerMarcoLibre();
			if(posicionMarco == -1)
				respuesta->resultado = ESPACIO_INSUFICIENTE_EN_FM9; //ERROR NO HAY MARCOS LIBRES

			ElementoTablaPag* elementoPagina = malloc(sizeof(ElementoTablaPag));
			elementoPagina->idPag = idPagina;
			idPagina++;
			elementoPagina->marco = posicionMarco;
			waitMutex(&mutexListaSegmentos);
			list_add(elementoSegmento->paginas, elementoPagina);
			signalMutex(&mutexListaSegmentos);
			waitMutex(&mutexListaPaginas);
			list_add(tablaDePaginas, elementoPagina);
			signalMutex(&mutexListaPaginas);
			int base = storage + posicionMarco * (tamanioPagina * tamanioLinea); //Porque el tamanioPagina esta en lineas
			if(cantidadPaginas - 1 == i){ //Es la ultima pagina
				for (int j = 0;  j < lineasEnLaUltimaPagina; j++) {
					if(lineas[lineaACargar]==NULL)
						lineas[lineaACargar] = string_new();

					string_append(&lineas[lineaACargar], "\n");
					char* textoAEscribir = malloc(tamanioLinea);
					memcpy(textoAEscribir, lineas[lineaACargar], strlen(lineas[lineaACargar]) + 1);
					memcpy(base + tamanioLinea * j, textoAEscribir, tamanioLinea); //Guardando de a una linea
					free(textoAEscribir);
					lineaACargar++;
				}
			}else{
				for(int j = 0; j < (tamanioPagina); j++){
					if(lineas[lineaACargar]==NULL)
						lineas[lineaACargar] = string_new();

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
		respuesta->elementoTabla = elementoSegmento;
		respuesta->pesoArchivo = cantidadPaginas * tamanioPagina;
	}

	freeLineas(lineas);
	return respuesta;
}

ElementoTablaPag* obtenerPaginasPorId(int pagina){
	bool coincideLaPagina(int elemento){
		if(elemento == pagina){
			return true;
		}
		return false;
	}
	waitMutex(&mutexListaPaginas);
	ElementoTablaPag* pag = list_find(tablaDePaginas, coincideLaPagina);
	signalMutex(&mutexListaPaginas);
	return pag;
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPag(int idDTB, char* nombreArchivo){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	ElementoTablaSegPag* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->segmentos);
	if(segmento != NULL){
		ElementoTablaPag* pagina;
		char* archivo = string_new();
		waitMutex(&mutexListaPaginas);
		for(int i=0; i < segmento->paginas->elements_count; i++){
			pagina = list_get(segmento->paginas, i);

			int desplazamiento = storage + pagina->marco * (tamanioPagina * tamanioLinea); //porque el tamanio de pag esta en lineas
			int hastaLinea;
			if(segmento->paginas->elements_count - 1 == i) //Es la ultima pagina
				hastaLinea = segmento->cantidadLineas % tamanioPagina;
			else
				hastaLinea = tamanioPagina;

			for (int j = 0;  j < hastaLinea; j++) {
				char* lineaConBasura = malloc(tamanioLinea);
				memcpy(lineaConBasura, desplazamiento + j * tamanioLinea, tamanioLinea);
				if(lineaConBasura[0] == '\n'){
					string_append(&archivo, "\n");
					free(lineaConBasura);
				}else{
					char** lineaSinBasura = string_split(lineaConBasura, "\n");
					string_append_with_format(&archivo, "%s\n", lineaSinBasura[0]);
					freeLineasBasura(lineaSinBasura, lineaConBasura);
				}
			}
		}signalMutex(&mutexListaPaginas);
		respuesta->datos = malloc(strlen(archivo) + 1);
		memcpy(respuesta->datos, archivo, strlen(archivo) + 1);
		free(archivo);
		respuesta->pudoObtener = 0;

	}else{
		log_error(logger, "No se encuentra el segmento de %s", nombreArchivo);
		respuesta->pudoObtener = FALLO_DE_SEGMENTO_MEMORIA; //ERROR
	}

	return respuesta;
}

respuestaDeObtencionDeMemoria* obtenerLineaSegPag(int idDTB, char* nombreArchivo, int numeroLinea){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	ElementoTablaSegPag* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->segmentos);

		if(numeroLinea < (segmento->cantidadLineas)){
			int paginaDondeSeEncuentraLaLinea = numeroLinea / tamanioPagina;
			int lineaDentroDeLaPagina = numeroLinea % tamanioPagina;
			waitMutex(&mutexListaPaginas);
			ElementoTablaPag* pagina = list_get(segmento->paginas, paginaDondeSeEncuentraLaLinea);
			signalMutex(&mutexListaPaginas);
			int desplazamientoPagina = pagina->marco * tamanioPagina * tamanioLinea; //Porque el tamanioPagina esta en lineas
			int desplazamientoLinea = lineaDentroDeLaPagina * tamanioLinea;
			char* lineaConBasura = asignarMemoria(tamanioLinea);
			memcpy(lineaConBasura, storage + desplazamientoPagina + desplazamientoLinea, tamanioLinea);
			if(lineaConBasura[0]=='\n'){ //FIN DE ARCHIVO
				respuesta->pudoObtener = 3;
				free(lineaConBasura);
			}else{
				char** lineaSinBasura = string_split(lineaConBasura, "\n");
				respuesta->datos = string_new();
				respuesta->pudoObtener = 0;
				string_append(&respuesta->datos, lineaSinBasura[0]);
				log_debug(logger, "Linea: %s", lineaSinBasura[0]);
				freeLineasBasura(lineaSinBasura, lineaConBasura);
			}
		}else{
			log_error(logger, "Error: El archivo no posee la linea %d", numeroLinea);
			respuesta->pudoObtener = 1; //ERROR
		}
	return respuesta;
}

int asignarDatosSegPag(int IdDTB, char* nombreArchivo, int numeroLinea, char* datos){
	numeroLinea--;
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(IdDTB);
	ElementoTablaSegPag* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->segmentos);
	if(numeroLinea < (segmento->cantidadLineas - 1)){
		int paginaDondeSeEncuentraLaLinea = numeroLinea / tamanioPagina;
		int lineaDentroDeLaPagina = numeroLinea % tamanioPagina;
		waitMutex(&mutexListaPaginas);
		ElementoTablaPag* pagina = list_get(segmento->paginas, paginaDondeSeEncuentraLaLinea);
		signalMutex(&mutexListaPaginas);
		int desplazamientoPagina = pagina->marco * tamanioPagina * tamanioLinea; //Porque el tamanioPagina esta en lineas
		int desplazamientoLinea = lineaDentroDeLaPagina * tamanioLinea;
		char* lineaConBasura = asignarMemoria(tamanioLinea);
		memcpy(lineaConBasura, storage + desplazamientoPagina + desplazamientoLinea, tamanioLinea);
		char** lineaSinBasura = string_split(lineaConBasura, "\n");
		char* lineaPosta;
		if(lineaSinBasura[0] == NULL)
			lineaPosta = string_new();
		else{
			lineaPosta = malloc(strlen(lineaSinBasura[0]));
			memcpy(lineaPosta, lineaSinBasura[0], strlen(lineaSinBasura[0]));
		}
		if((strlen(lineaPosta) + strlen(datos) + 2) < tamanioLinea){ //Lo que ya estaba, los datos nuevos, el /n y el espacio en el medio
			//Se puede escribir
			string_append_with_format(&lineaPosta, " %s\n", datos);
			log_debug("Linea resultante de la asignación: %s", lineaPosta);
			char* lineaAGuardar = malloc(tamanioLinea);
			memcpy(lineaAGuardar, lineaPosta, strlen(lineaPosta) + 1);
			string_trim(&lineaAGuardar);
			memcpy(storage + desplazamientoPagina + desplazamientoLinea, lineaAGuardar, tamanioLinea);
			freeLineasBasura(lineaSinBasura, lineaConBasura);
			free(lineaPosta);
			free(lineaAGuardar);
			log_debug(logger, "Asignados datos con exito");
			return 0;

		}else{
			freeLineasBasura(lineaSinBasura, lineaConBasura);
			free(lineaPosta);
			log_error(logger, "No hay suficiente espacio en la linea %d del archivo %s", (numeroLinea+1), nombreArchivo);
			return ESPACIO_INSUFICIENTE_EN_FM9; //ERROR
		}
	}else {
		if(segmento->cantidadLineas - 1 == numeroLinea){
			log_error(logger, "No se puede escribir en la ultima linea del archivo.");
			return FALLO_DE_SEGMENTO_MEMORIA; //ERROR
		}else{
			log_error(logger, "El DTB no posee la linea %d", numeroLinea);
			return FALLO_DE_SEGMENTO_MEMORIA; //ERROR
		}
	}
}

void liberarMemoriaSegPag(int idDTB, char* nombreArchivo){
	char* nombreA = malloc(strlen(nombreArchivo) + 1);
	memcpy(nombreA, nombreArchivo, strlen(nombreArchivo) + 1);

	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	//Primero libero en la tabla de paginas
	ElementoTablaSegPag* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->segmentos);
	waitMutex(&mutexListaPaginas);
	for (int i = 0; i < segmento->paginas->elements_count; ++i) {
		bool coincidePagina(ElementoTablaPag* elemeComparador){
			return elemeComparador->idPag == list_get(segmento->paginas, i);
		}
		list_remove_and_destroy_by_condition(tablaDePaginas, coincidePagina, free);
	}
	list_destroy(segmento->paginas);
	signalMutex(&mutexListaPaginas);
	//Ahora libero en la tabla de segmentos
	bool coincideNombre(ElementoTablaSegPag* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0){
			return true;
		}
		return false;
	}

	void destruirElemento(ElementoTablaSegPag* elemento){
		free(elemento->nombreArchivo);
		free(elemento);
	}

	waitMutex(&mutexListaSegmentos);
	list_remove_and_destroy_by_condition(proceso->segmentos, coincideNombre, destruirElemento);
	signalMutex(&mutexListaSegmentos);

	log_info(logger, "Borrado archivo %s de memoria", nombreA);
	free(nombreA);
}

void liberarDTBDeMemoriaSegPag(int idDTB){
	log_info(logger, "Liberando de la memoria el DTB");
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	for (int i = 0; i < proceso->segmentos->elements_count; ++i) {
		waitMutex(&mutexListaSegmentos);
		ElementoTablaSegPag* segmento = list_get(proceso->segmentos, i);
		signalMutex(&mutexListaSegmentos);
		liberarMemoriaSegPag(idDTB, segmento->nombreArchivo);
	}
	bool coincideIdDTB(ElementoTablaDTBS* elemento){
			if(elemento->idDTB == idDTB){
				return true;
			}
			return false;
		}

		void destruirElemento(ElementoTablaDTBS* elemento){
			list_destroy(elemento->segmentos);
			free(elemento);
		}
		waitMutex(&mutexListaProcesos);
	list_remove_and_destroy_by_condition(tablaDeProcesos, coincideIdDTB, destruirElemento);
	signalMutex(&mutexListaProcesos);
}

void dumpSegPag(int idDTB){
	log_info(logger, "Dump de DTB: %d", idDTB);
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	waitMutex(&mutexListaSegmentos);
	int cantidadDeSegmentos = proceso->segmentos->elements_count;
	signalMutex(&mutexListaSegmentos);
	log_info(logger, "El DTB con id %d, tiene %d archivos abiertos en memoria", idDTB, cantidadDeSegmentos);
	for (int i = 0; i < cantidadDeSegmentos; ++i) {
		waitMutex(&mutexListaSegmentos);
		ElementoTablaSegPag* segmento = list_get(proceso->segmentos, i);
		signalMutex(&mutexListaSegmentos);
		respuestaDeObtencionDeMemoria* respuesta = obtenerDatosSegPag(idDTB, segmento->nombreArchivo);
		log_info(logger, "El archivo %d tiene estos datos guardados: \n %s", (i+1), respuesta->datos);
		freeRespuestaObtencion(respuesta);
	}
}
