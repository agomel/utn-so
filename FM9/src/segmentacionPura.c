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
		}else{
			rompio = true;
			break;
		}

	if(!rompio){
	respuesta = 0;
	log_debug(logger, "Datos guardados");
	}

	return respuesta;
}

ElementoTablaSegPura* obtenerPorId(int idSegmento){
	bool coincideId(ElementoTablaSegPura* elemento){
		return elemento->id == idSegmento;
	}

	return list_find(tablaDeSegmentos, coincideId);
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPura(t_list* idsSegmentos){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	int tamanioTotal = 1;
	char* datos = malloc(tamanioTotal);
	bool rompio = false;

	for(int i=0; i<idsSegmentos->elements_count; i++){
		int id = list_get(idsSegmentos, i);
		ElementoTablaSegPura* segmento = obtenerPorId(id);
		if(segmento != NULL){
			tamanioTotal += segmento->limite;
			datos = realloc(datos, tamanioTotal);
			memcpy(datos, storage + segmento->base, segmento->limite);
		}else{
			rompio = true;
		}
	}

	if(!rompio){
		respuesta->datos = malloc(strlen(datos) + 1);
		memcpy(respuesta->datos, datos, tamanioTotal);
		respuesta->cantidadDeLineas = idsSegmentos->elements_count; //Asumiendo que se guarda una linea por segmento
		respuesta->pudoObtener = 0;
	}else{
		respuesta->pudoObtener = 1;
	}
	return respuesta;
}

respuestaDeObtencionDeMemoria* obtenerLineaSegPura(t_list* idsSegmentos, int numeroLinea){
	respuestaDeObtencionDeMemoria* respuesta = malloc(sizeof(respuestaDeObtencionDeMemoria));
	if(numeroLinea < idsSegmentos->elements_count){
		int id = list_get(idsSegmentos, numeroLinea);
		ElementoTablaSegPura* elemento = obtenerPorId(id);
		respuesta->cantidadDeLineas = 1;
		respuesta->datos = malloc(elemento->limite);
		respuesta->pudoObtener = 0;
		memcpy(respuesta->datos, storage + elemento->base, elemento->limite);
	}else{
		log_error(logger, "El DTB no posee la linea %d", numeroLinea);
		respuesta->pudoObtener = 1;
	}
	return respuesta;
}
