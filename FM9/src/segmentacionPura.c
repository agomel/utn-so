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

respuestaDeCargaEnMemoria guardarDatosSegPura(char* datos){
	//COMO SE SEGMENTA?? POR AHORA SE GUARDA TODO LO QUE LLEGÓ EN UN SEGMENTO
	log_debug(logger, "Guardando en paginacion pura");
	respuestaDeCargaEnMemoria respuesta;
	int tamanioSegmento = strlen(datos) + 1;

	int posicionDondeGuardar = dondeEntro(tamanioSegmento);

	if(posicionDondeGuardar != -1){
		ElementoTablaSegPura* elementoTabla = malloc(sizeof(ElementoTablaSegPura));
		elementoTabla->id = idSegmento;
		elementoTabla->base = posicionDondeGuardar;
		elementoTabla->limite = tamanioSegmento;
		list_add(tablaDeSegmentos, elementoTabla);

		t_list* idsSegmento = list_create();
		list_add(idsSegmento, idSegmento);

		memcpy(storage+posicionDondeGuardar, datos, tamanioSegmento);

		respuesta.listaDeDirecciones = idsSegmento;
		respuesta.pudoGuardarlo = 0;
		idSegmento++;

		log_debug(logger, "Datos guardados");
	}else{
		respuesta.pudoGuardarlo = 1;
	}

	return respuesta;
}

respuestaDeObtencionDeMemoria* obtenerDatosSegPura(t_list* posiciones){
	respuestaDeObtencionDeMemoria* respuesta;
	respuesta->cantidadDeLineas = 0;
	t_list* lista = list_create();
	respuesta->datos = lista;
	respuesta->pudoGuardarlo = 0;
	return respuesta;
}
