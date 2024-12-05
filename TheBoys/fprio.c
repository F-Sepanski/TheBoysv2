// TAD Fila de prioridades (FPRIO) genérica
// Carlos Maziero, DINF/UFPR, Out 2024
// Implementação com lista encadeada simples

// A COMPLETAR
#include <stdlib.h>
#include <stdio.h>
#include "fprio.h"

struct fprio_t *fprio_cria()
{
  struct fprio_t *fila = (struct fprio_t *)malloc(sizeof(struct fprio_t));
  if (!fila)
  {
    return NULL;
  }
  fila->num = 0;
  fila->prim = NULL;
  return fila;
}

struct fprio_t *fprio_destroi(struct fprio_t *fila)
{
  while (fila->prim != NULL)
  {
    struct fpnodo_t *primaux = fila->prim;
    fila->prim = primaux->prox;
    free(primaux->item);
    free(primaux);
  }
  free(fila);
  return NULL;
}

int fprio_insere(struct fprio_t *fila, void *item, int tipo, int prio)
{
  if (fila == NULL || item == NULL)
  {
    return -1;
  }

  // Verifica se o item já existe na fila
  struct fpnodo_t *current = fila->prim;
  while (current != NULL)
  {
    if (current->item == item)
    {
      return -1; // Item duplicado encontrado
    }
    current = current->prox;
  }

  struct fpnodo_t *nodo = (struct fpnodo_t *)malloc(sizeof(struct fpnodo_t));
  if (nodo == NULL)
  {
    return -1;
  }

  nodo->tipo = tipo;
  nodo->prio = prio;
  nodo->item = item;
  nodo->prox = NULL;

  if (fila->prim == NULL || fila->prim->prio > prio)
  {
    // insercao no inicio
    nodo->prox = fila->prim;
    fila->prim = nodo;
  }
  else
  {
    // insercao no meio ou fim
    current = fila->prim;
    while (current->prox != NULL && current->prox->prio <= prio)
    {
      current = current->prox;
    }
    nodo->prox = current->prox;
    current->prox = nodo;
  }

  fila->num++;
  return fila->num;
}

void *fprio_retira(struct fprio_t *fila, int *tipo, int *prio)
{
  if (fila == NULL || fila->prim == NULL || tipo == NULL || prio == NULL)
  {
    return NULL;
  }

  struct fpnodo_t *nodopos = fila->prim;

  fila->prim = nodopos->prox;

  *tipo = nodopos->tipo;
  *prio = nodopos->prio;

  void *item = nodopos->item;
  free(nodopos);
  fila->num--;

  return item;
}

int fprio_tamanho(struct fprio_t *fila)
{
  if (fila != NULL)
  {
    return fila->num;
  }
  else
    return -1;
}

void fprio_imprime(struct fprio_t *fila)
{
  if (fila == NULL)
  {
    return;
  }

  struct fpnodo_t *nodoaux = fila->prim;
  while (nodoaux != NULL)
  {
    printf("(%d %d) ", nodoaux->tipo, nodoaux->prio);
    nodoaux = nodoaux->prox;
  }
}
