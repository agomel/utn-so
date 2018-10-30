#include "segmentacionPag.h"
#include "stdbool.h"

void inicializarSegPag(){
	tablaDeSegmentos = list_create();
	idSegmento = 0;
	tablaDePaginas = list_create();
	idPagina = 0;
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

int guardarDatosSegPag(char* datos, char* nombreArchivo){
	log_debug(logger, "Guardando en paginacion pura");
	int respuesta = 1;
	int tamanioSegmento = strlen(datos) + 1;
	int cantidadPaginas = tamanioSegmento / tamanioPagina;
	int cantidadLineas = tamanioSegmento / tamanioLinea; //Limite

	if(tamanioSegmento % tamanioPagina != 0)
		cantidadPaginas++;

	if((cantidadMarcosTotales - tablaDePaginas->elements_count) >= cantidadPaginas){
		respuesta = 0; //No hay error
		ElementoTablaSeg* elementoSegmento = malloc(sizeof(ElementoTablaSeg));
		elementoSegmento->cantidadLineas = cantidadLineas;
		elementoSegmento->paginas = list_create();
		elementoSegmento->id = idSegmento;
		idSegmento++;

		for(int i = 0; i++; i < cantidadPaginas){
			char* textoAGuardar;
			int posicionMarco = obtenerMarcoLibre();

			int desplazamiento = i*tamanioLinea;

			if(cantidadPaginas - 1 == i){ //Es la ultima pagina
				textoAGuardar = string_substring(datos, desplazamiento, strlen(datos) - desplazamiento);
			}else{
				textoAGuardar = string_substring(datos, desplazamiento, tamanioPagina * tamanioLinea);
			}

			memcpy(storage + desplazamiento, textoAGuardar, tamanioPagina);

			list_add(elementoSegmento->paginas, idPagina); //Agrego la pagina a la lista de ese segmento
			ElementoTablaPag* elementoPag = malloc(sizeof(ElementoTablaPag));
			elementoPag->idPag = idPagina;
			elementoPag->marco = posicionMarco;
			list_add(tablaDePaginas, elementoPag);
			idPagina++;
		}
		list_add(tablaDeSegmentos, elementoSegmento);
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
	respuestaDeObtencionDeMemoria* respuesta;
		return respuesta;
}
void liberarMemoriaSegPag(char* nombreArchivo){
	log_info(logger, "liberando memoria");
}

