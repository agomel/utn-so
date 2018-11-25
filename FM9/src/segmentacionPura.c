#include "segmentacionPura.h"
#include "stdbool.h"

void inicializarSegPura(){
	tablaDeProcesos = list_create();
	segmentosOcupados = list_create();
	idSegmento = 0;
	inicializarMutex(&mutexListaProcesos);
	inicializarMutex(&mutexListaSegmentos);
}

static void freeRespuestaCargaSegPura(RespuestaCargaSegPura* respuesta);
static RespuestaCargaSegPura* guardarDatosInternaSegPura(char* datos, char* nombreArchivo);
static int dondeEntro(int tamanioAGuardar);
static ElementoTablaProcesos* obtenerProcesoPorIdDTB(int idDTB);

ElementoTablaProcesos* crearElemTablaProcesos(int idDTB, int tablaSegmentos){
	ElementoTablaProcesos* elemento = malloc(sizeof(ElementoTablaProcesos));
	elemento->idDTB = idDTB;
	elemento->tablaSegmentos = list_create();
	elemento->tablaSegmentos = tablaSegmentos;
	return elemento;
}

ElementoTablaSegPura* crearElemTablaSegPura(int base, int tamanioSegmento, char* nombreArchivo){
	ElementoTablaSegPura* elemento= malloc(sizeof(ElementoTablaSegPura));
	elemento->id = idSegmento;
	elemento->base = base;
	elemento->limite = tamanioSegmento;
	elemento->nombreArchivo = malloc(strlen(nombreArchivo)+1);
	memcpy(elemento->nombreArchivo, nombreArchivo, strlen(nombreArchivo)+1);
	return elemento;
}

void agregarASegmentoOcupado(int base, int limite){
	SegmentoOcupado* segmento = malloc(sizeof(SegmentoOcupado));
	segmento->base = base;
	segmento->limite = limite;
	list_add(segmentosOcupados, segmento);
}

static void copiarElemSegPura(ElementoTablaSegPura* de, ElementoTablaSegPura* hasta){
	int tamanioArchivo = strlen(de->nombreArchivo);
	hasta->nombreArchivo = malloc(tamanioArchivo + 1);
	memcpy(hasta->nombreArchivo, de->nombreArchivo, strlen(de->nombreArchivo) +1);
	hasta->nombreArchivo[tamanioArchivo] = '\0';
	hasta->base = de->base;
	hasta->id = de->id;
	hasta->limite = de->limite;
}

RespuestaGuardado* nuevoProcesoSegPura(int idDTB, char* datos, char* nombreArchivo){
	RespuestaCargaSegPura* cargaEnMemoria = guardarDatosInternaSegPura(datos, nombreArchivo);
	RespuestaGuardado* respuesta = malloc(sizeof(RespuestaGuardado));

	if(cargaEnMemoria->resultado == 0){ //No rompio
		t_list* tablaSegmentos = list_create();
		ElementoTablaSegPura* nuevoRegistro = malloc(sizeof(ElementoTablaSegPura));
		copiarElemSegPura(cargaEnMemoria->elementoTabla, nuevoRegistro);
		waitMutex(&mutexListaSegmentos);
		list_add(tablaSegmentos, nuevoRegistro);
		signalMutex(&mutexListaSegmentos);
		ElementoTablaProcesos* elementoTablaProcesos = crearElemTablaProcesos(idDTB, tablaSegmentos);
		waitMutex(&mutexListaProcesos);
		list_add(tablaDeProcesos, elementoTablaProcesos);
		signalMutex(&mutexListaProcesos);
		log_info(logger, "Agregado proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = 0;
		respuesta->pesoArchivo = cargaEnMemoria->pesoArchivo;
		freeRespuestaCargaSegPura(cargaEnMemoria);
	}else{
		log_error(logger, "No se pudo agregar proceso %d a tabla de procesos", idDTB);
		respuesta->pudoGuardar = cargaEnMemoria->resultado; //ERROR
	}

	return respuesta;
}

RespuestaGuardado* guardarDatosSegPura(int idDTB, char* datos, char* nombreArchivo){
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	RespuestaCargaSegPura* cargaEnMemoria = guardarDatosInternaSegPura(datos, nombreArchivo);
	RespuestaGuardado* respuesta = malloc(sizeof(RespuestaGuardado));
	if(cargaEnMemoria->resultado == 0){
		ElementoTablaSegPura* nuevoRegistro = malloc(sizeof(ElementoTablaSegPura));
		copiarElemSegPura(cargaEnMemoria->elementoTabla, nuevoRegistro);
		waitMutex(&mutexListaSegmentos);
		list_add(proceso->tablaSegmentos, nuevoRegistro);
		signalMutex(&mutexListaSegmentos);
		respuesta->pudoGuardar = 0;
		respuesta->pesoArchivo = cargaEnMemoria->pesoArchivo;
		freeRespuestaCargaSegPura(cargaEnMemoria);
	}else{
		respuesta->pudoGuardar = cargaEnMemoria->resultado; //ERROR
	}

	return respuesta;
}

static RespuestaCargaSegPura* guardarDatosInternaSegPura(char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en segmentacion pura");
	int rompio = FALLO_DE_SEGMENTO_MEMORIA;

	RespuestaCargaSegPura* respuesta = malloc(sizeof(RespuestaCargaSegPura));
	int totalLineas = cantidadDeLineas(datos);
	char** lineas = dividirPorLineas(datos);

	int tamanioSegmento = totalLineas * tamanioLinea;
	int posicionDondeGuardar = dondeEntro(tamanioSegmento);
	if(posicionDondeGuardar != -1){
		ElementoTablaSegPura* elementoTabla = crearElemTablaSegPura(posicionDondeGuardar, tamanioSegmento, nombreArchivo);
		respuesta->elementoTabla = elementoTabla;
		respuesta->pesoArchivo = totalLineas;
		agregarASegmentoOcupado(posicionDondeGuardar, tamanioSegmento);
		int base = storage + posicionDondeGuardar;
		for(int i = 0; i<totalLineas; i++){
			if(lineas[i]==NULL)
				lineas[i] = string_new();

			string_append(&lineas[i], "\n");
			char* textoAEscribir = asignarMemoria(tamanioLinea);
			memcpy(textoAEscribir, lineas[i], strlen(lineas[i]) + 1);
			memcpy(base + tamanioLinea * i, textoAEscribir, tamanioLinea); //Guardando de a una linea
			free(textoAEscribir);
		}
		idSegmento++;
		rompio = 0;
		log_debug(logger, "Datos guardados");
	}

	freeLineas(lineas);
	respuesta->resultado = rompio;
	return respuesta;
}

static ElementoTablaProcesos* obtenerProcesoPorIdDTB(int idDTB){
	bool coincideId(ElementoTablaProcesos* elemento){
		return elemento->idDTB == idDTB;
	}
	waitMutex(&mutexListaProcesos);
	ElementoTablaProcesos* proceso = list_find(tablaDeProcesos, coincideId);
	signalMutex(&mutexListaProcesos);
	return proceso;
}

static ElementoTablaSegPura* obtenerSegmentoPorArchivo(char* nombreArchivo, t_list* tablaSegmentos){
	bool coincideNombre(ElementoTablaSegPura* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0)
			return true;

		return false;
	}
	waitMutex(&mutexListaSegmentos);
	ElementoTablaSegPura* segmento = list_find(tablaSegmentos, coincideNombre);
	signalMutex(&mutexListaSegmentos);
	return segmento;
}

static void freeRespuestaCargaSegPura(RespuestaCargaSegPura* respuesta){
	free(respuesta->elementoTabla->nombreArchivo);
	free(respuesta->elementoTabla);
	free(respuesta);
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPura(int idDTB, char* nombreArchivo){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	ElementoTablaSegPura* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->tablaSegmentos);
	if(segmento != NULL){
		int cantidadLineas = segmento->limite / tamanioLinea;
		char* archivo = string_new();
		for(int i=0; i<cantidadLineas; i++){
			int desplazamiento = segmento->base + i * tamanioLinea;
			char* lineaConBasura = malloc(tamanioLinea);
			memcpy(lineaConBasura, storage + desplazamiento, tamanioLinea);
			if(lineaConBasura[0] == '\n'){ //Ultima linea archivo
				string_append(&archivo, "\n");
				free(lineaConBasura);
			}else{
				char** lineaSinBasura = string_split(lineaConBasura, "\n");
				string_append_with_format(&archivo, "%s\n", lineaSinBasura[0]);
				freeLineasBasura(lineaSinBasura, lineaConBasura);
			}
		}
		respuesta->datos = malloc(strlen(archivo) + 1);
		memcpy(respuesta->datos, archivo, strlen(archivo) + 1);
		free(archivo);
	}else{
		log_error(logger, "Error obteniendo el segmento de %s", nombreArchivo);
		respuesta->pudoObtener = FALLO_DE_SEGMENTO_MEMORIA; //ERROR
	}
		return respuesta;
}

respuestaDeObtencionDeMemoria* obtenerLineaSegPura(int idDTB, char* nombreArchivo, int numeroLinea){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	ElementoTablaSegPura* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->tablaSegmentos);
	int cantidadLineasDeArchivo = 1;
	if(segmento->limite > tamanioLinea)
		cantidadLineasDeArchivo = segmento->limite / tamanioLinea;

	if(numeroLinea < cantidadLineasDeArchivo){
		int desplazamiento = segmento->base + numeroLinea * tamanioLinea;
		char* lineaConBasura = malloc(tamanioLinea);
		memcpy(lineaConBasura, storage + desplazamiento, tamanioLinea);
		if(lineaConBasura[0]=='\n' || lineaConBasura[0] == NULL){ //FIN DE ARCHIVO
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
		log_error(logger, "El DTB no posee la linea %d", numeroLinea);
		respuesta->pudoObtener = 1; //ERROR
	}

	return respuesta;
}

void liberarMemoriaSegPura(int idDTB, char* nombreArchivo){
	int base;
	log_info(logger, "Borrado archivo %s de memoria", nombreArchivo);
	bool coincideNombre(ElementoTablaSegPura* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0){
			base = elemento->base;
			return true;
		}
		return false;
	}

	void destruirElemento(ElementoTablaSegPura* elemento){
		free(elemento->nombreArchivo);
		free(elemento);
	}

	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	waitMutex(&mutexListaSegmentos);
	list_remove_and_destroy_by_condition(proceso->tablaSegmentos, coincideNombre, destruirElemento);
	signalMutex(&mutexListaSegmentos);

	bool coincideBase(SegmentoOcupado* segmento){
		return segmento->base == base;
	}

	list_remove_and_destroy_by_condition(segmentosOcupados, coincideBase, free);
}

void liberarDTBDeMemoriaSegPura(int idDTB){
	log_info(logger, "Liberando de la memoria el DTB");
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	for (int i = 0; i < proceso->tablaSegmentos->elements_count; ++i) {
		waitMutex(&mutexListaSegmentos);
		ElementoTablaSegPura* segmento = list_get(proceso->tablaSegmentos, i);
		signalMutex(&mutexListaSegmentos);
		liberarMemoriaSegPura(idDTB, segmento->nombreArchivo);
	}
	bool coincideIdDTB(ElementoTablaProcesos* elemento){
			if(elemento->idDTB == idDTB){
				return true;
			}
			return false;
		}

		void destruirElemento(ElementoTablaProcesos* elemento){
			waitMutex(&mutexListaSegmentos);
			list_destroy(elemento->tablaSegmentos);
			signalMutex(&mutexListaSegmentos);
			free(elemento);
		}
		waitMutex(&mutexListaProcesos);
	list_remove_and_destroy_by_condition(tablaDeProcesos, coincideIdDTB, destruirElemento);
	signalMutex(&mutexListaProcesos);
}

int asignarDatosSegPura(int IdDTB, char* nombreArchivo, int numeroLinea, char* datos){
	numeroLinea--;
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(IdDTB);
	ElementoTablaSegPura* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->tablaSegmentos);
	int cantidadLineasSegmento = segmento->limite / tamanioLinea;

	if(numeroLinea < cantidadLineasSegmento - 1){
		int desplazamiento = segmento->base + numeroLinea * tamanioLinea;
		char* lineaConBasura = malloc(tamanioLinea);
		memcpy(lineaConBasura, storage + desplazamiento, tamanioLinea);
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
			memcpy(storage + desplazamiento, lineaAGuardar, tamanioLinea);
			if(lineaSinBasura[0] == NULL)
				free(lineaConBasura);
			else
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
	}else{
		if(cantidadLineasSegmento - 1 == numeroLinea){
			log_error(logger, "No se puede escribir en la ultima linea del archivo");
		}else{
			log_error(logger, "El archivo no posee la linea %d", numeroLinea);
		}
		return FALLO_DE_SEGMENTO_MEMORIA; //ERROR
	}
}


static bool compararElementos(SegmentoOcupado* elem1, SegmentoOcupado* elem2){
	return elem1->base < elem2->base;
}

static int dondeEntro(int tamanioAGuardar){
	if(segmentosOcupados->elements_count >= 1){

	if(segmentosOcupados->elements_count > 1)
	list_sort(segmentosOcupados, compararElementos);

	SegmentoOcupado* primerElemento = list_get(segmentosOcupados, 0);
	int espacioDelPrincipio = primerElemento->base - 1;
	if(espacioDelPrincipio >= tamanioAGuardar)
		return 0;

	for(int i = 0; i < segmentosOcupados->elements_count-1; i++){
		SegmentoOcupado* elem1 = list_get(segmentosOcupados, i);
		SegmentoOcupado* elem2 = list_get(segmentosOcupados, i+1);

		int posicionInicial = elem1->base + elem1->limite + 1;
		int tamanioEntreAmbos = (elem2->base - 1) - posicionInicial;

		if(tamanioEntreAmbos >= tamanioAGuardar)//Entra en ese lugar
			return posicionInicial;
	}

	SegmentoOcupado* ultimoElemento = list_get(segmentosOcupados, segmentosOcupados->elements_count -1);
	int ultimaPosicion = ultimoElemento->base + ultimoElemento->limite;
	int tamanioRestanteEnMemoria = tamanioMemoria - ultimaPosicion;

	if(tamanioRestanteEnMemoria >= tamanioAGuardar){
		return ultimaPosicion + 1;
	}

	log_error(logger, "No hay suficiente espacio en memoria");
	return -1;
	}

	return 0;
}

void dumpSegPura(int idDTB){
	log_info(logger, "Dump de DTB: %d", idDTB);
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	waitMutex(&mutexListaSegmentos);
	int cantidadDeSegmentos = proceso->tablaSegmentos->elements_count;
	signalMutex(&mutexListaSegmentos);
	log_info(logger, "El DTB con id %d, tiene %d archivos abiertos en memoria", idDTB, cantidadDeSegmentos);
	for (int i = 0; i < cantidadDeSegmentos; ++i) {
		waitMutex(&mutexListaSegmentos);
		ElementoTablaSegPura* segmento = list_get(proceso->tablaSegmentos, i);
		signalMutex(&mutexListaSegmentos);
		respuestaDeObtencionDeMemoria* respuesta = obtenerDatosSegPura(idDTB, segmento->nombreArchivo);
		log_info(logger, "El archivo %d tiene estos datos guardados: \n %s", (i+1), respuesta->datos);
		freeRespuestaObtencion(respuesta);
	}
}
