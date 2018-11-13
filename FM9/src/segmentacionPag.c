#include "segmentacionPag.h"
#include "stdbool.h"

void inicializarSegPag(){
	tablaDeSegmentos = list_create();
	idSegmento = 0;
	tablaDePaginas = list_create();
	idPagina = 0;
	tablaDeProcesos = list_create();
}

static ElementoTablaDTBS* obtenerProcesoPorIdDTB(int idDTB){
	bool coincideId(ElementoTablaDTBS* elemento){
		return elemento->idDTB == idDTB;
	}

	return list_find(tablaDeProcesos, coincideId);
}

bool compararPorMarco(ElementoTablaPag* elem1, ElementoTablaPag* elem2){
	return elem1->marco < elem2->marco;
}

int obtenerMarcoLibre(){
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
		return ultimoElemento->marco + 1;
	}
	return 0;
}

ElementoTablaSeg* crearElemTablaSegPag(char* nombreArchivo, int cantLineas){
	ElementoTablaSeg* elemento = malloc(sizeof(ElementoTablaSeg));
	elemento->cantidadLineas = cantLineas;
	elemento->id = idSegmento;
	elemento->nombreArchivo = nombreArchivo;
	elemento->paginas = list_create();
	idSegmento++;
	return elemento;
}

ElementoTablaDTBS* crearElemTablaDTBS(int idDTB, t_list* tablaSegmentos){
	ElementoTablaDTBS* elemento = malloc(sizeof(ElementoTablaDTBS));
	elemento->idDTB = idDTB;
	elemento->segmentos = list_create();
	elemento->segmentos = tablaSegmentos;
	return elemento;
}

static void freeRespuestaCargaSegPag(RespuestaCargaSegPag* respuesta){
	free(respuesta->elementoTabla->nombreArchivo);
	free(respuesta->elementoTabla);
	free(respuesta);
}

RespuestaGuardado* nuevoProcesoSegPag(int idDTB, char* datos, char* nombreArchivo){
	RespuestaCargaSegPag* cargaEnMemoria = guardarDatosInternaSegPag(datos, nombreArchivo);
	RespuestaGuardado* respuesta = malloc(sizeof(RespuestaGuardado));

	if(cargaEnMemoria->resultado == 0){ //No rompio
		t_list* tablaSegmentos = list_create();
		list_add(tablaSegmentos, cargaEnMemoria->elementoTabla);
		list_add(tablaDeSegmentos, cargaEnMemoria->elementoTabla);
		ElementoTablaDTBS* elementoTablaDTBS = crearElemTablaDTBS(idDTB, tablaSegmentos);
		list_add(tablaDeProcesos, elementoTablaDTBS);
		log_info(logger, "Agregado proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = 0;
		respuesta->pesoArchivo = cargaEnMemoria->pesoArchivo;
		freeRespuestaCargaSegPag(cargaEnMemoria);
	}else{
		log_error(logger, "No se pudo agregar proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = cargaEnMemoria->resultado;
	}
	return respuesta;
}

RespuestaGuardado* guardarDatosSegPag(int idDTB, char* datos, char* nombreArchivo){
	ElementoTablaDTBS* proceso = obtenerProcesoPorIdDTB(idDTB);
	RespuestaCargaSegPag* cargaEnMemoria = guardarDatosInternaSegPag(datos, nombreArchivo);
	RespuestaGuardado* respuesta = malloc(sizeof(RespuestaGuardado));
	if(cargaEnMemoria->resultado == 0){
		list_add(proceso->segmentos, cargaEnMemoria->elementoTabla);
		respuesta->pudoGuardar = 0;
		respuesta->pesoArchivo = cargaEnMemoria->pesoArchivo;
		freeRespuestaCargaSegPag(cargaEnMemoria);
	}else{
		respuesta->pudoGuardar = cargaEnMemoria->resultado;
	}
	return respuesta;
}

RespuestaCargaSegPag* guardarDatosInternaSegPag(char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en segmentacion paginada");

	RespuestaCargaSegPag* respuesta = malloc(sizeof(RespuestaCargaSegPag));
	respuesta->resultado = 1;
	int totalLineas = cantidadDeLineas(datos);
	char** lineas = string_split(datos, "\n");
	int tamanioSegmento = totalLineas * tamanioLinea;
	int cantidadPaginas = tamanioSegmento / tamanioPagina;
	int lineasEnLaUltimaPagina = tamanioSegmento % tamanioPagina;
	if(lineasEnLaUltimaPagina != 0)
		cantidadPaginas++;

	if((cantidadMarcosTotales - tablaDePaginas->elements_count) >= cantidadPaginas){
		respuesta->resultado = 0; //No hay error
		ElementoTablaSeg* elementoSegmento = crearElemTablaSegPag(nombreArchivo, totalLineas);
		int lineaACargar = 0;
		for(int i = 0; i++; i < cantidadPaginas){
			char* textoAGuardar;
			int posicionMarco = obtenerMarcoLibre();
			ElementoTablaPag* elementoPagina = malloc(sizeof(ElementoTablaPag));
			elementoPagina->idPag = idPagina;
			idPagina++;
			elementoPagina->marco = posicionMarco;
			list_add(elementoSegmento->paginas, elementoPagina);
			list_add(tablaDePaginas, elementoPagina);
			int base = storage + posicionMarco * tamanioPagina;
			if(cantidadPaginas - 1 != i){ //Es la ultima pagina
				for (int j = 0;  j < lineasEnLaUltimaPagina; j++) {
					string_append(&lineas[lineaACargar], "\n");
					memcpy(base + tamanioLinea * j, lineas[lineaACargar], tamanioLinea); //Guardando de a una linea
					lineaACargar++;
				}
			}else{
				for(int j = 0; j < (tamanioPagina/tamanioLinea); j++){
					string_append(&lineas[lineaACargar], "\n");
					memcpy(base + tamanioLinea * j, lineas[lineaACargar], tamanioLinea); //Guardando de a una linea
					lineaACargar++;
				}
			}
		}
		log_debug(logger, "Datos guardados");
		respuesta->elementoTabla = elementoSegmento;
		respuesta->pesoArchivo = cantidadPaginas * tamanioPagina;
	}
	return respuesta;
}

ElementoTablaSeg* obtenerPorNombreArchivoPaginada(char* nombreArchivo){
	bool coincideNombre(ElementoTablaSeg* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0){
			return true;
		}
		return false;
	}
	return list_find(tablaDeSegmentos, coincideNombre);
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

respuestaDeObtencionDeMemoria* obtenerDatosSegPag(char* nombreArchivo){
	int paginaActual;
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaSeg* elemento = obtenerPorNombreArchivoPaginada(nombreArchivo);
	ElementoTablaPag* elemPag;
	respuesta->datos = asignarMemoria(elemento->cantidadLineas * tamanioLinea);
	for (int i = 0; i < elemento->paginas->elements_count-1; i++) {
		paginaActual = list_get(elemento->paginas, i);
		elemPag = obtenerPaginasPorId(paginaActual);
		memcpy(respuesta->datos + i*tamanioPagina,storage + elemPag->marco, tamanioPagina);
	}
	int lineasRestantes = elemento->cantidadLineas - (elemento->cantidadLineas % (elemento->paginas->elements_count * tamanioPagina));
	int tamanioDelSegmento = (elemento->cantidadLineas - lineasRestantes) * tamanioLinea;
	elemPag = obtenerPaginasPorId(list_get(elemento->paginas, elemento->paginas->elements_count-1));
	memcpy(respuesta->datos + tamanioDelSegmento, storage + elemPag->marco, lineasRestantes*tamanioLinea);
	respuesta->cantidadDeLineas = elemento->cantidadLineas;

	return respuesta;
}

respuestaDeObtencionDeMemoria* obtenerLineaSegPag(char* nombreArchivo, int numeroLinea){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaSeg* elemento = obtenerPorNombreArchivoPaginada(nombreArchivo);
	if(numeroLinea < elemento->cantidadLineas){
		int cantidadDeLineasPorPagina = tamanioPagina / tamanioLinea;
		int paginaDondeSeEncuentraLaLinea = numeroLinea / cantidadDeLineasPorPagina;
		int lineaDentroDeLaPagina = numeroLinea % cantidadDeLineasPorPagina;
		if(lineaDentroDeLaPagina != 0)
			paginaDondeSeEncuentraLaLinea++;
		ElementoTablaPag* elemPag = obtenerPaginasPorId(paginaDondeSeEncuentraLaLinea);
		int desplazamiento = lineaDentroDeLaPagina * tamanioLinea;
		char* lineaConBasura = asignarMemoria(tamanioLinea);
		memcpy(lineaConBasura, storage + elemPag->marco + desplazamiento, tamanioLinea);
		char** lineaSinBasura = string_split(lineaConBasura, "\n");
		respuesta->cantidadDeLineas = 1;
		respuesta->datos = malloc(strlen(lineaSinBasura[0])+1);
		respuesta->pudoObtener = 0;
		memcpy(respuesta->datos, lineaSinBasura[0], strlen(lineaSinBasura[0])+1);
	}else{
		log_error(logger, "El DTB no posee la linea %d", numeroLinea);
		respuesta->pudoObtener = 1;
	}
	return respuesta;
}

void liberarMemoriaSegPag(char* nombreArchivo){
	//Primero libero en la tabla de paginas
	ElementoTablaSeg* elemento = obtenerPorNombreArchivoPaginada(nombreArchivo);
	for (int i = 0; i < elemento->paginas->elements_count; ++i) {
		bool coincidePagina(ElementoTablaPag* elemeComparador){
			return elemeComparador->idPag == list_get(elemento->paginas, i);
		}
		list_remove_and_destroy_by_condition(tablaDePaginas, coincidePagina, free);
	}
	//Ahora libero en la tabla de segmentos
	bool coincideNombre(ElementoTablaSeg* elemento){
			if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0){
				return true;
			}
			return false;
		}

		void destruirElemento(ElementoTablaSeg* elemento){
			free(elemento->nombreArchivo);
			free(elemento);
		}

		list_remove_and_destroy_by_condition(tablaDeSegmentos, coincideNombre, destruirElemento);
	log_info(logger, "liberando memoria");
}

