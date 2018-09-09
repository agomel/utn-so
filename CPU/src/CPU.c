
#include <biblioteca/socket.h>

int main(void) {
	int cliente = conectarConServidor(20001, inet_addr("127.0.0.1"));

	while(1){
		char mensaje[1000];
		scanf("%s", mensaje);
		enviarMensaje(cliente, mensaje);
	}
	return 0;
}
