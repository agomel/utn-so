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
char* serializarMensaje(int operacion,char*mensaje){
	char* serializado = asignarMemoria(strlen(mensaje)+6);
	int tamanioMensaje= strlen(mensaje);
	if(operacion<10){
		strcat(serializado, "0");
	}
	sprintf(serializado, "%d", operacion);

	if(tamanioMensaje<10){
			strcat(serializado, "0");
	}
	sprintf(serializado, "%d", tamanioMensaje);
	strcat(serializado, mensaje);
	strcat(serializado, "99");
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
