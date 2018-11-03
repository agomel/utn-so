#include "segmentacionPura.h"
#include "stdbool.h"

void inicializarSegPura(){
	tablaDeSegmentos = list_create();
	idSegmento = 0;
}

bool compararElementos(ElementoTablaSegPura* elem1, ElementoTablaSegPura* elem2){
	return elem1->base < elem2->base;
}

int dondeEntro(int tamanioAGuardar){
	if(tablaDeSegmentos->elements_count >= 1){

	if(tablaDeSegmentos->elements_count > 1)
	list_sort(tablaDeSegmentos, compararElementos);

	ElementoTablaSegPura* primerElemento = list_get(tablaDeSegmentos, 0);
	int espacioDelPrincipio = primerElemento->base;
	if(espacioDelPrincipio >= tamanioAGuardar)
		return 0;

	for(int i = 0; i++; i < tablaDeSegmentos->elements_count-1){
		ElementoTablaSegPura* elem1 = list_get(tablaDeSegmentos, i);
		ElementoTablaSegPura* elem2 = list_get(tablaDeSegmentos, i+1);

		int posicionInicial = elem1->base + elem1->limite;
		int tamanioEntreAmbos = elem2->base - posicionInicial;

		if(tamanioEntreAmbos >= tamanioAGuardar){
			//Entra en ese lugar
			return posicionInicial;
		}
	}

	ElementoTablaSegPura* ultimoElemento = list_get(tablaDeSegmentos, tablaDeSegmentos->elements_count -1);
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

int guardarDatosSegPura(char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en paginacion pura");
	int respuesta = 1;
	bool rompio = false;

	int tamanioSegmento = strlen(datos) + 1;
	int posicionDondeGuardar = dondeEntro(tamanioSegmento);
	if(posicionDondeGuardar != -1){
			ElementoTablaSegPura* elementoTabla = malloc(sizeof(ElementoTablaSegPura));
			elementoTabla->id = idSegmento;
			elementoTabla->base = posicionDondeGuardar;
			elementoTabla->limite = tamanioSegmento;
			elementoTabla->nombreArchivo = malloc(strlen(nombreArchivo)+1);
			memcpy(elementoTabla->nombreArchivo, nombreArchivo, strlen(nombreArchivo)+1);
			list_add(tablaDeSegmentos, elementoTabla);
			memcpy(storage + posicionDondeGuardar, datos, tamanioSegmento);
			idSegmento++;
			respuesta = 0;
			log_debug(logger, "Datos guardados");
		}

	return respuesta;
}

ElementoTablaSegPura* obtenerPorNombreArchivo(char* nombreArchivo){
	bool coincideNombre(ElementoTablaSegPura* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0){
			return true;
		}
		return false;
	}

	return list_find(tablaDeSegmentos, coincideNombre);
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPura(char* nombreArchivo){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaSegPura* elemento = obtenerPorNombreArchivo(nombreArchivo);
	respuesta->datos = malloc(elemento->limite);
	memcpy(respuesta->datos, storage + elemento->base, elemento->limite);
	respuesta->cantidadDeLineas = elemento->limite / tamanioLinea;

	return respuesta;
}

respuestaDeObtencionDeMemoria* obtenerLineaSegPura(char* nombreArchivo, int numeroLinea){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	ElementoTablaSegPura* elemento = obtenerPorNombreArchivo(nombreArchivo);
	int cantidadLineasDeArchivo = 1;
	if(elemento->limite > tamanioLinea)
		cantidadLineasDeArchivo = elemento->limite / tamanioLinea;

	if(numeroLinea < cantidadLineasDeArchivo){
		int desplazamiento = numeroLinea * tamanioLinea;
		char* lineaConBasura = malloc(tamanioLinea);
		memcpy(lineaConBasura, storage + desplazamiento, tamanioLinea);
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

void liberarMemoriaSegPura(char* nombreArchivo){
	bool coincideNombre(ElementoTablaSegPura* elemento){
		if(strcmp(elemento->nombreArchivo, nombreArchivo) == 0){
			return true;
		}
		return false;
	}

	void destruirElemento(ElementoTablaSegPura* elemento){
		free(elemento->nombreArchivo);
		free(elemento);
	}

	list_remove_and_destroy_by_condition(tablaDeSegmentos, coincideNombre, destruirElemento);
	log_info(logger, "liberando memoria");
}

