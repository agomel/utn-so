#ifndef LISTAS_H_
#define LISTAS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "planificador.h"

DTB* obtenerDTBDeCola(int idDTB);

DTB* obtenerDTBDeColaRemoviendolo(int idDTB);

t_list* filtrarListaPorEstado(char estado);

DTB* obtenerPrimerDTBEnNew();

void agregarDTBALista(DTB* dtb);

void pasarDTBAlFinalDeLista(int idDTB);

DTB* removerDTBPorIndice(int indice);

cambiarEstadoDummy(char estado);

int obtenerCPUDisponibleYOcupar(int id);

#endif /*LISTAS_H_*/
