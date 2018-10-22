#include "segmentacionPura.h"

int idSegmento = 0;

respuestaDeCargaEnMemoria guardarDatosSegPura(char* datos){
	//COMO SE SEGMENTA?? POR AHORA SE GUARDA TODO LO QUE LLEGÓ EN UN SEGMENTO

	log_debug(logger, "Guardando en paginacion pura");
	respuestaDeCargaEnMemoria respuesta;
	int tamanioSegmento = strlen(datos) + 1;
	ElementoTablaSegPura* elementoTabla = malloc(sizeof(ElementoTablaSegPura));
	elementoTabla->id = idSegmento;
	elementoTabla->limite = tamanioSegmento;
	elementoTabla->base = offset;

	list_add(tablaDeSegmentos, elementoTabla);

	t_list* numerosSegmento = list_create();
	list_add(numerosSegmento, idSegmento);

	memcpy(storage, datos, tamanioSegmento);

	offset = offset + tamanioSegmento;

	idSegmento++;

	respuesta.listaDeDirecciones = numerosSegmento;
	respuesta.pudoGuardarlo = 0;

	return respuesta;
}

respuestaDeObtencionDeMemoria obtenerDatosSegPura(t_list* posiciones){
	respuestaDeObtencionDeMemoria* respuesta;
	respuesta->cantidadDeLineas = 0;
	t_list* lista = list_create();
	respuesta->datos = lista;
	respuesta->pudoGuardarlo = 0;
	return respuesta;
}
