#ifndef DAM_H_
#define DAM_H_

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/select.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/dtb.h>
#include <commons/collections/queue.h>

int transferSize;

int socketFM9;
int socketMDJ;
int socketSAFA;
t_queue* colaOperaciones;

pthread_mutex_t mutexColaOperaciones;
sem_t semHayEnColaOperaciones;
t_config * archivoConfig;

typedef struct{
	char* path;
	int idDTB;
	int offset;
	int size;
	char accion;
}Operacion;

void inicializarDAM();
void enviarAMDJ(Operacion* operacion);
void agregarOperacionACola(int emisor, char accion);
int enviarDatosAFM9(char* datos);
void recibirDatosDeFM9(void* buffer, int* desplazamiento);
void verificarDatosDeMDJYEnviarASafa(Operacion* operacion);
void consumirCola();
void escucharCPU(int socketCPU);
int main(void);


#endif /*DAM_H_*/
