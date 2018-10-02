#ifndef DAM_H_
#define DAM_H_

#include <biblioteca/socket.h>
#include <biblioteca/hilos.h>
#include <biblioteca/utilidades.h>
#include <biblioteca/select.h>
#include <biblioteca/serializacion.h>
#include <biblioteca/dtb.h>
#include <commons/collections/queue.h>

u_int32_t socketFM9;
u_int32_t socketMDJ;
u_int32_t socketSAFA;
t_queue* colaOperaciones;

t_config* archivoConfiguracion;
pthread_mutex_t mutexColaOperaciones;
sem_t semHayEnColaOperaciones;

typedef struct{
	char* path;
	u_int32_t idDTB;
	u_int32_t offset;
	u_int32_t size;
	char accion;
}Operacion;

void inicializarDAM();
void enviarAMDJ(Operacion operacion);
void agregarOperacionACola(int emisor, char accion);
void enviarDatosAFM9(char* datos);
void recibirDatosDeFM9(void* buffer, u_int32_t* desplazamiento);
void consumirCola();
void escucharCPU(int socketCPU);
int main(void);


#endif /*DAM_H_*/
