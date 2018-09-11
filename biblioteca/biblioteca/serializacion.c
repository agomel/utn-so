#include "serializacion.h"



void deserializar(char** parametros,int emisor){
	int espacioALeer;

	int parametro=0;
	char* buffer;

	buffer=asignarMemoria(3);
	recibirMensaje(emisor, &buffer, 2);
	espacioALeer = atoi(buffer);
	while(espacioALeer != 99){
		parametros[parametro]=asignarMemoria(espacioALeer +1);
		recibirMensaje(emisor, &parametros[parametro], espacioALeer);

		free(buffer);
		buffer=asignarMemoria(3);
		recibirMensaje(emisor, &buffer, 2);
		espacioALeer = atoi(buffer);
		parametro++;
	}
}
char* serializarMensajeAEnviar(char* mensaje){
	char* serializado;
	char* tamanioMensaje;
	tamanioMensaje=malloc(2);
	int longitudMensaje=strlen(mensaje);
	if(longitudMensaje<10){
		strcat(tamanioMensaje,"0");
	}
	tamanioMensaje=longitudMensaje;
	serializado=asignarMemoria(6+tamanioMensaje);
	strcat(serializado,"02");
	strcat(serializado,tamanioMensaje);
	strcat(serializado,mensaje);
	strcat(serializado,"99");
	return serializado;
}


void deserializarIdentificarse(int emisor,t_dictionary* conexiones){
	char* parametros[1];
	printf("Esto es el identificador \n");
	deserializar(parametros,emisor);
	printf("Me mandaron %s \n",parametros[0]);
	dictionary_put(conexiones,parametros[0],emisor);
	printf("Agregado %s a las conexiones \n",parametros[0]);
}
