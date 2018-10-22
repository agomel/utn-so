#ifndef SEGMENTACIONPAG_H_
#define SEGMENTACIONPAG_H_

#include "FM9.h"

respuestaDeCargaEnMemoria guardarDatosSegPag(char* datos);
respuestaDeObtencionDeMemoria* obtenerDatosSegPag(t_list* posiciones);

#endif /* SEGMENTACIONPAG_H_ */
