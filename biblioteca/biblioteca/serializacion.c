#include "serializacion.h"


/*
void deserializar(void** parametros,int emisor){
	int espacioALeer;

	int parametro=0;
	void* buffer;

	buffer=asignarMemoria(2);
	recibirMensaje(emisor, &buffer, 2);
	espacioALeer = atoi(buffer);
	while(espacioALeer != 99){
		parametros[parametro]=asignarMemoria(espacioALeer);
		recibirMensaje(emisor, &parametros[parametro], espacioALeer);

		free(buffer);
		buffer=asignarMemoria(2);
		recibirMensaje(emisor, &buffer, 2);
		espacioALeer = atoi(buffer);
		parametro++;
	}
}
char* serializarMensaje(int operacion,char*mensaje){
	char* serializado = asignarMemoria(strlen(mensaje)+6);
	int tamanioMensaje= strlen(mensaje);
	if(operacion<10 && tamanioMensaje<10){
		sprintf(serializado, "0%d0%d",operacion,tamanioMensaje);
	}else if(operacion>10 && tamanioMensaje<10){
		sprintf(serializado, "%d0%d",operacion,tamanioMensaje);
	}else if(operacion<10 && tamanioMensaje>10){
		sprintf(serializado, "0%d%d",operacion,tamanioMensaje);
	}else{
		sprintf(serializado, "%d%d",operacion,tamanioMensaje);
	}
	strcat(serializado,mensaje);

	strcat(serializado,"99");
	return serializado;
}
void deserializarIdentificarse(int emisor,t_dictionary* conexiones){
	char* parametros[1];
	printf("Esto es el identificador \n");
	deserializar((void**)parametros,emisor);
	printf("Me mandaron %s \n",parametros[0]);
	dictionary_put(conexiones,parametros[0],emisor);
	printf("Agregado %s a las conexiones \n",parametros[0]);
}
*/

void handshake(u_int32_t servidor, char modulo){
	u_int32_t tamanioMensaje = sizeof(char)*2;
	void* mensaje = asignarMemoria(tamanioMensaje);
	char operacion = IDENTIFICARSE;
	u_int32_t puntero = 0;
	memcpy(mensaje + puntero, &operacion, sizeof(char));
	puntero = puntero + sizeof(char);
	memcpy(mensaje + puntero, &modulo, sizeof(char));
	puntero = puntero + sizeof(char); //este paso no es necesario ya que no vuelvo a usar el puntero
	enviarMensaje(servidor, mensaje, tamanioMensaje);
	free(mensaje);
}

char deserializarIdentificarse(u_int32_t emisor){
	char modulo;
	recibirMensaje(emisor, &modulo, sizeof(char));
	printf("Se identifico a %c \n" , modulo);
	return modulo;
}

void enviarStringSerializado(u_int32_t destino, char* texto){
	u_int32_t tamanioTexto = strlen(texto) + 1;
	u_int32_t tamanioMensaje = sizeof(char) + sizeof(u_int32_t) + tamanioTexto;
	void* mensaje = asignarMemoria(tamanioMensaje);

	char operacion = MANDAR_TEXTO;
	u_int32_t puntero = 0;

	concatenarString(mensaje, puntero, operacion);
	/*memcpy(mensaje + puntero, &operacion, sizeof(char));
	puntero = puntero + sizeof(char);
	 */
	memcpy(mensaje + puntero, &tamanioTexto, sizeof(u_int32_t));
	puntero = puntero + sizeof(u_int32_t);

	memcpy(mensaje + puntero, texto, tamanioTexto);
	puntero = puntero + tamanioTexto; //esta linea no es necesaria ya que no vuelvo a utilizar el puntero

	enviarMensaje(destino, mensaje, tamanioMensaje);

	free(mensaje);
}

void concatenarString(void* buffer, u_int32_t* desplazamiento, char* mensaje){
	memcpy(buffer + desplazamiento, mensaje, sizeof(char));
	desplazamiento = desplazamiento + sizeof(char);
}
int concatenarInt(void* buffer, u_int32_t* desplazamiento, u_int32_t numero){
	memcpy(buffer + desplazamiento, &numero, sizeof(u_int32_t));
	desplazamiento = desplazamiento + sizeof(u_int32_t);
}

void deserializarString(u_int32_t emisor){
	u_int32_t tamanioMensaje;
	recibirMensaje(emisor, &tamanioMensaje, sizeof(u_int32_t));
	char* mensaje = malloc(tamanioMensaje);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	printf("Recibi %s de parte de %d \n" , mensaje, emisor);
}

/*void serializarMensajito(mensajito mensaje){
	void* m = asignarMemoria(sizeof(mensaje));
	char operacion = MANDAR_MENSAJITO;



}*/


