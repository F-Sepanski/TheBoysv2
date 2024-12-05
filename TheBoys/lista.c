// TAD lista de números inteiros
// Carlos Maziero - DINF/UFPR, Out 2024
//
// Implementação do TAD - a completar
//
// Implementação com lista encadeada dupla não-circular
#include <stdlib.h>
#include <stdio.h>
#include "lista.h"

struct lista_t *lista_cria()
{
  struct lista_t *lista = (struct lista_t *)malloc(sizeof(struct lista_t));
  if (!lista)
  {
    return NULL;
  }
  lista->prim = NULL;
  lista->ult = NULL;
  lista->tamanho = 0;
  return lista;
}

struct lista_t *lista_destroi(struct lista_t *lst)
{
  if (lst == NULL)
  {
    return NULL;
  }

  struct item_t *item = lst->prim;
  struct item_t *itemprox;
  while (item != NULL)
  {
    itemprox = item->prox;
    free(item);
    item = itemprox;
  }
  free(lst);
  return NULL;
}

int lista_insere(struct lista_t *lst, int item, int pos)
{
  if (pos < -1 || lst == NULL)
  {
    return -1;
  }

  struct item_t *itempos;
  if (pos > lst->tamanho || pos == -1)
  {
    itempos = lst->ult; // insere no final
  }
  else
  {
    itempos = lst->prim;
    for (int i = 0; i < pos; i++)
    {
      itempos = itempos->prox;
    }
  }

  struct item_t *item_nodo = (struct item_t *)malloc(sizeof(struct item_t));
  if (!item_nodo)
  {
    return -1;
  }
  item_nodo->valor = item;

  if (lst->prim == NULL) // lista vazia
  {
    item_nodo->ant = NULL;
    item_nodo->prox = NULL;
    lst->prim = item_nodo;
    lst->ult = item_nodo;
  }
  else if (itempos == lst->prim) // insere no inicio
  {
    item_nodo->prox = lst->prim;
    item_nodo->ant = NULL;
    lst->prim->ant = item_nodo;
    lst->prim = item_nodo;
  }
  else if (itempos == lst->ult) // insere no final
  {
    item_nodo->prox = NULL;
    item_nodo->ant = lst->ult;
    lst->ult->prox = item_nodo;
    lst->ult = item_nodo;
  }
  else // insere no meio
  {
    item_nodo->prox = itempos;
    item_nodo->ant = itempos->ant;
    itempos->ant->prox = item_nodo;
    itempos->ant = item_nodo;
  }

  lst->tamanho++;
  return lst->tamanho;
}

int lista_retira(struct lista_t *lst, int *item, int pos)
{
  if (lst == NULL || item == NULL || pos < -1)
  {
    return -1;
  }

  struct item_t *itempos;
  if (pos == -1 || pos >= lst->tamanho)
  {
    itempos = lst->ult; // retira do final
  }
  else
  {
    itempos = lst->prim;
    for (int i = 0; i < pos; i++)
    {
      itempos = itempos->prox;
    }
  }

  if (itempos == NULL)
  {
    return -1;
  }

  if (itempos->ant != NULL)
  {
    itempos->ant->prox = itempos->prox;
  }
  else
  {
    lst->prim = itempos->prox; // atualiza o primeiro item
  }

  if (itempos->prox != NULL)
  {
    itempos->prox->ant = itempos->ant;
  }
  else
  {
    lst->ult = itempos->ant; // atualiza o ultimo item
  }

  *item = itempos->valor;
  free(itempos);
  lst->tamanho--;

  return lst->tamanho;
}

int lista_consulta(struct lista_t *lst, int *item, int pos)
{
  if (lst == NULL || item == NULL || pos < -1)
  {
    return -1;
  }

  struct item_t *itempos;
  if (pos == -1 || pos >= lst->tamanho)
  {
    itempos = lst->ult; // consulta o ultimo item
  }
  else
  {
    itempos = lst->prim;
    for (int i = 0; i < pos; i++)
    {
      itempos = itempos->prox;
    }
  }

  if (itempos == NULL)
  {
    return -1;
  }

  *item = itempos->valor;
  return lst->tamanho;
}

int lista_procura(struct lista_t *lst, int valor)
{
  if (lst == NULL)
  {
    return -1;
  }

  struct item_t *item = lst->prim;
  for (int i = 0; i < lst->tamanho; i++)
  {
    if (item == NULL)
    {
      return -1;
    }

    if (item->valor == valor)
    {
      return i;
    }

    item = item->prox;
  }

  return -1;
}

int lista_tamanho(struct lista_t *lst)
{
  if (!lst)
  {
    return -1;
  }
  return lst->tamanho;
}

void lista_imprime(struct lista_t *lst)
{
  if (lst == NULL)
  {
    printf("Lista vazia.\n");
    return;
  }

  struct item_t *epos = lst->prim;
  while (epos != NULL)
  {
    printf("%d ", epos->valor);
    epos = epos->prox;
  }
}
