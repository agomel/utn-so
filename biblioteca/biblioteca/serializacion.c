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
char* enviarIdentificacion(char* mensaje){
	char* serializado = malloc(strlen(mensaje)+6);
	int tamanioNombre = strlen(mensaje);
	sprintf(serializado, "02%d", tamanioNombre);
	strcat(serializado, mensaje);
	strcat(serializado, "99");
	return serializado;
}


char* serializarMensajeAEnviar(char* mensaje){
	char* serializado;
	int longitudMensaje=strlen(mensaje);
	serializado=asignarMemoria(6+longitudMensaje);

	strcat(serializado,"02");
	if(longitudMensaje<10){
		strcat(serializado,"0");
	}
	strcat(serializado,longitudMensaje);
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
