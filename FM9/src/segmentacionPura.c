#include "segmentacionPura.h"
#include "stdbool.h"

void inicializarSegPura(){
	tablaDeProcesos = list_create();
	segmentosOcupados = list_create();
	idSegmento = 0;
}

static ElementoTablaProcesos* obtenerProcesoPorIdDTB(int idDTB);
static void freeRespuestaCargaSegPura(RespuestaCargaSegPura* respuesta);
static RespuestaCargaSegPura* guardarDatosInternaSegPura(char* datos, char* nombreArchivo);
static int dondeEntro(int tamanioAGuardar);
static void freeLineasBasura(char** lineaSinBasura, char* lineaConBasura);

ElementoTablaProcesos* crearElemTablaProcesos(int idDTB, int tablaSegmentos){
	ElementoTablaProcesos* elemento = malloc(sizeof(ElementoTablaProcesos));
	elemento->idDTB = idDTB;
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
	hasta->nombreArchivo = malloc(strlen(de->nombreArchivo) + 1);
	memcpy(hasta->nombreArchivo, de->nombreArchivo, strlen(de->nombreArchivo) +1);
	hasta->base = de->base;
	hasta->id = de->id;
	hasta->limite = de->limite;
}

int nuevoProcesoSegPura(int idDTB, char* datos, char* nombreArchivo){
	RespuestaCargaSegPura* respuestaCarga = guardarDatosInternaSegPura(datos, nombreArchivo);

	if(respuestaCarga->respuestaGuardado == 0){ //No rompio
		t_list* tablaSegmentos = list_create();
		ElementoTablaSegPura* nuevoRegistro = malloc(sizeof(ElementoTablaSegPura));
		copiarElemSegPura(respuestaCarga->elementoTabla, nuevoRegistro);
		list_add(tablaSegmentos, nuevoRegistro);
		freeRespuestaCargaSegPura(respuestaCarga);
		ElementoTablaProcesos* elementoTablaProcesos = crearElemTablaProcesos(idDTB, tablaSegmentos);
		list_add(tablaDeProcesos, elementoTablaProcesos);
		log_info(logger, "Agregado proceso %d a tabla de procesos", idDTB);
		return 0;
	}else{
		log_error(logger, "No se pudo agregar proceso %d a tabla de procesos", idDTB);
		int rta = respuestaCarga->respuestaGuardado;
		free(respuestaCarga);
		return rta;
	}
}

int guardarDatosSegPura(int idDTB, char* datos, char* nombreArchivo){
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	RespuestaCargaSegPura* cargaEnMemoria = guardarDatosInternaSegPura(datos, nombreArchivo);
	if(cargaEnMemoria->respuestaGuardado == 0){
		ElementoTablaSegPura* nuevoRegistro = malloc(sizeof(ElementoTablaSegPura));
		copiarElemSegPura(cargaEnMemoria->elementoTabla, nuevoRegistro);
		list_add(proceso->tablaSegmentos, nuevoRegistro);
		freeRespuestaCargaSegPura(cargaEnMemoria);
		return 0;
	}else{
		int rta = cargaEnMemoria->respuestaGuardado;
		free(cargaEnMemoria);
		return rta;
	}
}

static RespuestaCargaSegPura* guardarDatosInternaSegPura(char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en paginacion pura");
	int rompio = 1;

	RespuestaCargaSegPura* respuesta = malloc(sizeof(RespuestaCargaSegPura));
	int totalLineas = cantidadDeLineas(datos);
	char** lineas = string_split(datos, "\n");
	int tamanioSegmento = totalLineas * tamanioLinea;
	int posicionDondeGuardar = dondeEntro(tamanioSegmento);
	if(posicionDondeGuardar != -1){
		ElementoTablaSegPura* elementoTabla = crearElemTablaSegPura(posicionDondeGuardar, tamanioSegmento, nombreArchivo);
		respuesta->elementoTabla = elementoTabla;
		agregarASegmentoOcupado(posicionDondeGuardar, tamanioSegmento);
		int base = storage + posicionDondeGuardar;
		for(int i = 0; i<totalLineas; i++){
			memcpy(base + tamanioLinea * i, lineas[i], tamanioLinea); //Guardando de a una linea
		}
		idSegmento++;
		rompio = 0;
		log_debug(logger, "Datos guardados");
		}

	respuesta->respuestaGuardado = rompio;
	return respuesta;
}

static ElementoTablaProcesos* obtenerProcesoPorIdDTB(int idDTB){
	bool coincideId(ElementoTablaProcesos* elemento){
		return elemento->idDTB == idDTB;
	}

	return list_find(tablaDeProcesos, coincideId);
}

static ElementoTablaSegPura* obtenerSegmentoPorArchivo(char* nombreArchivo, t_list* tablaSegmentos){
	bool coincideNombre(ElementoTablaSegPura* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0)
			return true;

		return false;
	}

	return list_find(tablaSegmentos, coincideNombre);
}

static void freeRespuestaCargaSegPura(RespuestaCargaSegPura* respuesta){
	free(respuesta->elementoTabla);
	free(respuesta);
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPura(int idDTB, char* nombreArchivo){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(idDTB);
	ElementoTablaSegPura* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->tablaSegmentos);
	int cantidadLineas = segmento->limite / tamanioLinea;
	char* archivo = malloc(tamanioLinea);
	for(int i=0; i<cantidadLineas; i++){
		int desplazamiento = i * tamanioLinea;
		char* lineaConBasura = malloc(tamanioLinea);
		memcpy(lineaConBasura, storage + desplazamiento, tamanioLinea);
		char** lineaSinBasura = string_split(lineaConBasura, "\n");
		string_append_with_format(&archivo, "%s\n", lineaSinBasura[0]);
		freeLineasBasura(lineaSinBasura, lineaConBasura);
	}

	respuesta->datos = malloc(strlen(archivo));
	memcpy(respuesta->datos, archivo, strlen(archivo));
	respuesta->cantidadDeLineas = cantidadLineas;

	return respuesta;
}

static void freeLineasBasura(char** lineaSinBasura, char* lineaConBasura){
	free(lineaConBasura);
	free(lineaSinBasura[0]);
	free(lineaSinBasura[1]);
	free(lineaSinBasura);
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
		char** lineaSinBasura = string_split(lineaConBasura, "\n");
		respuesta->cantidadDeLineas = 1;
		respuesta->datos = malloc(strlen(lineaSinBasura[0])+1);
		respuesta->pudoObtener = 0;
		memcpy(respuesta->datos, lineaSinBasura[0], strlen(lineaSinBasura[0])+1);
		freeLineasBasura(lineaSinBasura, lineaConBasura);
	}else{
		log_error(logger, "El DTB no posee la linea %d", numeroLinea);
		respuesta->pudoObtener = 1;
	}

	return respuesta;
}

void liberarMemoriaSegPura(int idDTB, char* nombreArchivo){
	int base;
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
	list_remove_and_destroy_by_condition(proceso->tablaSegmentos, coincideNombre, destruirElemento);

	bool coincideBase(SegmentoOcupado* segmento){
		return segmento->base == base;
	}

	list_remove_and_destroy_by_condition(segmentosOcupados, coincideBase, free);
	log_info(logger, "liberando memoria");
}

int asignarDatosSegPura(int IdDTB, char* nombreArchivo, int numeroLinea, char* datos){
	ElementoTablaProcesos* proceso = obtenerProcesoPorIdDTB(IdDTB);
	ElementoTablaSegPura* segmento = obtenerSegmentoPorArchivo(nombreArchivo, proceso->tablaSegmentos);
	int desplazamiento = segmento->base + numeroLinea * tamanioLinea;
	char* lineaConBasura = malloc(tamanioLinea);
	memcpy(lineaConBasura, storage + desplazamiento, tamanioLinea);
	char** lineaSinBasura = string_split(lineaConBasura, "\n");
	char* lineaPosta = malloc(strlen(lineaSinBasura[0]));
	memcpy(lineaPosta, lineaSinBasura[0], strlen(lineaSinBasura[0]));
	if((strlen(lineaSinBasura[0] + strlen(datos)) + 2) < tamanioLinea){ //Lo que ya estaba, los datos nuevos, el /n y el espacio en el medio
		//Se puede escribir
		string_append_with_format(&lineaPosta, " %s\n", datos);
		memcpy(storage + desplazamiento, lineaPosta, strlen(lineaPosta));
		freeLineasBasura(lineaSinBasura, lineaConBasura);
		free(lineaPosta);
		log_debug(logger, "Asignados datos con exito");
		return 0;

	}else{
		log_error(logger, "No hay suficiente espacio en la linea %d del archivo %s", numeroLinea, nombreArchivo);
		freeLineasBasura(lineaSinBasura, lineaConBasura);
		free(lineaPosta);
		return 20002;
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
	int espacioDelPrincipio = primerElemento->base;
	if(espacioDelPrincipio >= tamanioAGuardar)
		return 0;

	for(int i = 0; i < segmentosOcupados->elements_count-1; i++){
		SegmentoOcupado* elem1 = list_get(segmentosOcupados, i);
		SegmentoOcupado* elem2 = list_get(segmentosOcupados, i+1);

		int posicionInicial = elem1->base + elem1->limite;
		int tamanioEntreAmbos = elem2->base - posicionInicial;

		if(tamanioEntreAmbos >= tamanioAGuardar){
			//Entra en ese lugar
			return posicionInicial;
		}
	}

	SegmentoOcupado* ultimoElemento = list_get(segmentosOcupados, segmentosOcupados->elements_count -1);
	int ultimaPosicion = ultimoElemento->base + ultimoElemento->limite;
	int tamanioRestanteEnMemoria = tamanioMemoria - ultimaPosicion;

	if(tamanioRestanteEnMemoria >= tamanioAGuardar){
		return ultimaPosicion;
	}

	log_error(logger, "No hay suficiente espacio en memoria");
	return -1;
	}

	return 0;
}
