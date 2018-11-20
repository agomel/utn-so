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
}

static ElementoTablaDTBS* obtenerProcesoPorIdDTB(int idDTB){
	bool coincideId(ElementoTablaDTBS* elemento){
		return elemento->idDTB == idDTB;
	}

	return list_find(tablaDeProcesos, coincideId);
}

static ElementoTablaSegPag* obtenerSegmentoPorArchivo(char* nombreArchivo, t_list* tablaSegmentos){
	bool coincideNombre(ElementoTablaSegPag* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0)
			return true;

		return false;
	}

	return list_find(tablaSegmentos, coincideNombre);
}

bool compararPorMarco(ElementoTablaPag* elem1, ElementoTablaPag* elem2){
	return elem1->marco < elem2->marco;
}

static int obtenerMarcoLibre(){
	if(tablaDePaginas->elements_count >= 1){

		if(tablaDePaginas->elements_count > 1)
			list_sort(tablaDePaginas, compararPorMarco);

		ElementoTablaPag* primerElemento = list_get(tablaDePaginas, 0);
		if(primerElemento->marco != 0)
			return 0;

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
		list_add(tablaSegmentos, nuevoRegistro);
		ElementoTablaDTBS* elementoTablaDTBS = crearElemTablaDTBS(idDTB, tablaSegmentos);
		list_add(tablaDeProcesos, elementoTablaDTBS);
		log_info(logger, "Agregado proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = 0;
		respuesta->pesoArchivo = cargaEnMemoria->pesoArchivo;
		freeRespuestaCargaSegPag(cargaEnMemoria);
	}else{
		log_error(logger, "No se pudo agregar proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = cargaEnMemoria->resultado;log_error(logger, "No se puede escribir en la ultima linea del archivo.");
		return 2000; //Error que corresponda
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
		list_add(proceso->segmentos, nuevoRegistro);
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
	if(totalLineas > tamanioPagina){
		cantidadPaginas = totalLineas / tamanioPagina;
		if(lineasEnLaUltimaPagina != 0)
				cantidadPaginas++;
	}

	if((cantidadMarcosTotales - tablaDePaginas->elements_count) >= cantidadPaginas){
		respuesta->resultado = 0; //No hay error
		ElementoTablaSegPag* elementoSegmento = crearElemTablaSegPag(nombreArchivo, totalLineas);
		int lineaACargar = 0;
		for(int i = 0; i < cantidadPaginas; i++){
			char* textoAGuardar;
			int posicionMarco = obtenerMarcoLibre();
			if(posicionMarco == -1)
				respuesta->resultado = -1; //ERROR NO HAY MARCOS LIBRES

			ElementoTablaPag* elementoPagina = malloc(sizeof(ElementoTablaPag));
			elementoPagina->idPag = idPagina;
			idPagina++;
			elementoPagina->marco = posicionMarco;
			list_add(elementoSegmento->paginas, elementoPagina);
			list_add(tablaDePaginas, elementoPagina);
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
	return list_find(tablaDePaginas, coincideLaPagina);
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPag(int idDTB, char* nombreArchivo){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	ElementoTablaSegPag* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->segmentos);
	if(segmento != NULL){
		ElementoTablaPag* pagina;
		char* archivo = string_new();
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
		}
		respuesta->datos = malloc(strlen(archivo) + 1);
		memcpy(respuesta->datos, archivo, strlen(archivo) + 1);
		free(archivo);
		respuesta->pudoObtener = 0;

	}else{
		log_error(logger, "El archivo %s no se encuentra abierto", nombreArchivo);
		respuesta->pudoObtener = 12444; //ERROR
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
			ElementoTablaPag* pagina = list_get(segmento->paginas, paginaDondeSeEncuentraLaLinea);
			int desplazamientoPagina = pagina->marco * tamanioPagina * tamanioLinea; //Porque el tamanioPagina esta en lineas
			int desplazamientoLinea = lineaDentroDeLaPagina * tamanioLinea;
			char* lineaConBasura = asignarMemoria(tamanioLinea);
			memcpy(lineaConBasura, storage + desplazamientoPagina + desplazamientoLinea, tamanioLinea);
			if(lineaConBasura[0]=='\n'){ //FIN DE ARCHIVO
				respuesta->pudoObtener = 3;
				free(lineaConBasura);
			}else{
				log_debug(logger, "En obtener: Linea: %s", lineaConBasura);
				char** lineaSinBasura = string_split(lineaConBasura, "\n");
				respuesta->datos = string_new();
				respuesta->pudoObtener = 0;
				string_append(&respuesta->datos, lineaSinBasura[0]);
				freeLineasBasura(lineaSinBasura, lineaConBasura);
			}
		}else{
			log_error(logger, "El DTB no posee la linea %d", numeroLinea);
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
		if((lineaDentroDeLaPagina != 0) && (numeroLinea < tamanioPagina))
			paginaDondeSeEncuentraLaLinea++;
		ElementoTablaPag* pagina = list_get(segmento->paginas, paginaDondeSeEncuentraLaLinea);
		int desplazamientoPagina = pagina->marco * tamanioPagina * tamanioLinea; //Porque el tamanioPagina esta en lineas
		int desplazamientoLinea = lineaDentroDeLaPagina * tamanioLinea;
		char* lineaConBasura = asignarMemoria(tamanioLinea);
		memcpy(lineaConBasura, storage + desplazamientoPagina + desplazamientoLinea, tamanioLinea);
		log_debug(logger, "En asignar: Linea: %s", lineaConBasura);
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
			if(segmento->cantidadLineas - 1 == numeroLinea){
				log_error(logger, "No se puede escribir en la ultima linea del archivo.");
				return 2000; //ERROR
			}else{
				log_error(logger, "No hay suficiente espacio en la linea %d del archivo %s", (numeroLinea+1), nombreArchivo);
				return 20002; //ERROR
			}
		}
	}else {
		log_error(logger, "El DTB no posee la linea %d", numeroLinea);
		return 30000; //ERROR
	}
}

void liberarMemoriaSegPag(int idDTB, char* nombreArchivo){
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	//Primero libero en la tabla de paginas
	ElementoTablaSegPag* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->segmentos);
	for (int i = 0; i < segmento->paginas->elements_count; ++i) {
		bool coincidePagina(ElementoTablaPag* elemeComparador){
			return elemeComparador->idPag == list_get(segmento->paginas, i);
		}
		list_remove_and_destroy_by_condition(tablaDePaginas, coincidePagina, free);
	}
	list_destroy(segmento->paginas);
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

	list_remove_and_destroy_by_condition(proceso->segmentos, coincideNombre, destruirElemento);

	log_info(logger, "liberando memoria");
}

void liberarDTBDeMemoriaSegPag(int idDTB){
	log_info(logger, "Liberando de la memoria el DTB");
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	for (int i = 0; i < proceso->segmentos->elements_count; ++i) {
		ElementoTablaSegPag* segmento = list_get(proceso->segmentos, i);
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
	list_remove_and_destroy_by_condition(tablaDeProcesos, coincideIdDTB, destruirElemento);
}
