#pragma once

#include "conjunto.h"
#include "fprio.h"
#include "lista.h"

#define T_INICIO 0
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS (N_HABILIDADES * 5)
#define N_BASES (N_HEROIS / 5)
#define N_MISSOES (T_FIM_DO_MUNDO / 100)

// Definição de tipos de eventos
#define CHEGA 0
#define ESPERA 1
#define DESISTE 2
#define AVISA 3
#define ENTRA 4
#define SAI 5
#define VIAJA 6
#define MORRE 7
#define MISSAO 8
#define FIM 9

// Declaracao de estruturas
typedef struct
{
    unsigned int x;
    unsigned int y;
} posicao;

typedef struct
{
    unsigned int id;
    struct cjto_t *habilidades; // Conjunto de habilidades que o herói possui
    unsigned int paciencia;     // Indica quão paciente o herói é
    unsigned int velocidade;    // Velocidade de deslocamento do herói
    unsigned int experiencia;   // Número de missões em que o herói já participou
    unsigned int base;          // ID da base onde o herói se encontra no momento
    bool vivo;
} heroi;

typedef struct
{
    unsigned int id;
    unsigned int lotacao;     // Número máximo de heróis na base
    struct cjto_t *presentes; // Conjunto dos IDs dos heróis presentes na base
    struct lista_t *espera;   // Fila de espera para entrar na base
    posicao local;            // Localização da base (coordenadas X, Y)
} base;

typedef struct
{
    unsigned int id;
    struct cjto_t *habilidades; // Conjunto de habilidades necessárias para cumprir a missão
    unsigned int perigo;        // Nível de perigo da missão
    posicao local;              // Localização da missão (coordenadas X, Y)
    bool cumprida;
    int tentativas;
} missao;

typedef struct
{
    unsigned int tempo;
    int tipo;
    heroi *heroi;
    base *base;
    missao *missao;
} evento;

typedef struct
{
    unsigned int num_herois;      // Número total de heróis no mundo
    heroi *herois;                // Vetor representando todos os heróis
    unsigned int num_bases;       // Número total de bases no mundo
    base *bases;                  // Vetor representando todas as bases
    unsigned int num_missoes;     // Número total de missões a cumprir
    missao *missoes;              // Vetor representando todas as missões
    unsigned int num_habilidades; // Número de habilidades distintas possíveis
    posicao tamanho;              // Coordenadas máximas do plano cartesiano (tamanho do mundo)
    unsigned int relogio;         // Tempo atual no mundo (em minutos)
    unsigned int missoes_cumpridas;
    unsigned int herois_mortos;
} mundo;

// Declaracao de funcoes de eventos
int chega(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef);
void espera(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef);
void desiste(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef, mundo *mundo_atual);
void avisa(unsigned int tempo, base *base, struct fprio_t *lef, mundo *mundo_atual);
int entra(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef);
void sai(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef, mundo *mundo_atual);
void viaja(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef, mundo *mundo_atual);
void morre(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef, mundo *mundo_atual);
void missao_ev(unsigned int tempo, missao *missao_atual, struct fprio_t *lef, mundo *mundo_atual);
// Declaracao de funcoes de eventos de mundo
void inicializa_mundo(mundo *mundo, struct fprio_t *lef);
void finaliza_mundo(mundo *mundo, struct fprio_t *lef);
