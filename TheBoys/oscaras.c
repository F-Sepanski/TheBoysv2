#include "oscaras.h"
#include <stdlib.h>
#include <stdio.h>

// Funcoes auxiliares
int random_int(int min, int max)
{
  int rd_num = rand() % (max - min + 1) + min;
  return rd_num;
}

double sqrt_custom(double x)
{
  if (x < 0)
    return 0;
  if (x == 0)
    return 0;

  double guess = x / 2;
  double epsilon = 0.000001; // Precisao

  while (1)
  {
    double new_guess = (guess + x / guess) / 2;

    double diff = new_guess - guess;
    if (diff < 0)
      diff = -diff;
    if (diff < epsilon)
    {
      return new_guess;
    }

    guess = new_guess;
  }
}

double calcular_distancia(posicao p1, posicao p2)
{
  int dx = (int)p2.x - (int)p1.x;
  int dy = (int)p2.y - (int)p1.y;
  return sqrt_custom(dx * dx + dy * dy);
}

void cria_evento(struct fprio_t *lef, unsigned int tempo, int tipo, heroi *heroi, base *base, missao *missao)
{
  evento *novo_evento = (evento *)(malloc(sizeof(evento)));
  if (novo_evento == NULL)
  {
    fprintf(stderr, "Erro ao ao alocar memoria para o evento\n");
    exit(1);
  }
  novo_evento->tempo = tempo;
  novo_evento->tipo = tipo;
  novo_evento->heroi = heroi;
  novo_evento->base = base;
  novo_evento->missao = missao;
  fprio_insere(lef, novo_evento, tipo, tempo);
};

// Funcoes de eventos de mundo
void inicializa_herois(mundo *mundo_atual)
{
  for (unsigned int i = 0; i < mundo_atual->num_herois; i++)
  {
    heroi *heroi_atual = &mundo_atual->herois[i];
    heroi_atual->id = i;
    heroi_atual->base = 0;
    heroi_atual->experiencia = 0;
    heroi_atual->paciencia = (unsigned int)random_int(0, 100);
    heroi_atual->velocidade = (unsigned int)random_int(50, 5000);
    heroi_atual->vivo = true;
    heroi_atual->habilidades = cjto_aleat(3, mundo_atual->num_habilidades);
  }
}

void inicializa_bases(mundo *mundo_atual)
{
  for (unsigned int i = 0; i < mundo_atual->num_bases; i++)
  {
    base *base_atual = &mundo_atual->bases[i];
    base_atual->id = i;
    base_atual->local.x = (unsigned int)random_int(0, mundo_atual->tamanho.x);
    base_atual->local.y = (unsigned int)random_int(0, mundo_atual->tamanho.y);
    base_atual->lotacao = (unsigned int)random_int(3, 10);
    base_atual->presentes = cjto_cria(N_HEROIS);
    if (!base_atual->presentes)
    {
      fprintf(stderr, "Erro ao criar conjunto 'presentes' para a base %u \n", i);
      exit(EXIT_FAILURE);
    }
    base_atual->espera = lista_cria();
    if (!base_atual->espera)
    {
      fprintf(stderr, "Erro ao criar lista 'espera' para a base %u \n", i);
      exit(EXIT_FAILURE);
    }
  }
}

void inicializa_missoes(mundo *mundo_atual)
{
  for (unsigned int i = 0; i < mundo_atual->num_missoes; i++)
  {
    missao *missao_atual = &mundo_atual->missoes[i];
    missao_atual->id = i;
    missao_atual->local.x = (unsigned int)random_int(0, mundo_atual->tamanho.x);
    missao_atual->local.y = (unsigned int)random_int(0, mundo_atual->tamanho.y);
    int n_habilidades = random_int(6, 10);
    missao_atual->habilidades = cjto_aleat(n_habilidades, N_HABILIDADES);
    missao_atual->perigo = (unsigned int)random_int(0, 100);
    missao_atual->cumprida = false;
    missao_atual->tentativas = 0;
  }
}

void inicializa_mundo(mundo *mundo_atual, struct fprio_t *lef)
{
  inicializa_herois(mundo_atual);
  inicializa_bases(mundo_atual);
  inicializa_missoes(mundo_atual);
  mundo_atual->herois_mortos = 0;
  mundo_atual->missoes_cumpridas = 0;

  for (unsigned int i = 0; i < mundo_atual->num_herois; i++)
  {
    int random_base = random_int(0, mundo_atual->num_bases - 1);
    mundo_atual->herois[i].base = random_base;
    int random_tempo = random_int(0, 4320);

    // Atualize o ponteiro para o herói e a base
    heroi *heroi_atual = &mundo_atual->herois[i];
    base *base_atual = &mundo_atual->bases[random_base];

    cria_evento(lef, (unsigned int)random_tempo, CHEGA, heroi_atual, base_atual, NULL);
  }
  for (unsigned int i = 0; i < mundo_atual->num_missoes; i++)
  {
    int random_tempo_de_missao = random_int(0, T_FIM_DO_MUNDO);
    cria_evento(lef, random_tempo_de_missao, MISSAO, NULL, NULL, &mundo_atual->missoes[i]);
  }

  cria_evento(lef, T_FIM_DO_MUNDO, FIM, NULL, NULL, NULL);
}

// Funcoes de eventos
int chega(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef)
{
  heroi->base = base->id;

  // Verifica se há vagas na base e se a fila de espera está vazia
  int espera = 0;
  if (lista_tamanho(base->espera) == 0 && (int)base->lotacao > cjto_card(base->presentes))
  {
    espera = 1;
  }
  else
  {
    espera = (int)heroi->paciencia > (10 * lista_tamanho(base->espera));
  }

  // Cria e insere o evento apropriado na LEF
  if (espera)
  {
    cria_evento(lef, tempo, ESPERA, heroi, base, NULL);
    return 0;
  }
  else
  {
    cria_evento(lef, tempo, DESISTE, heroi, base, NULL);
    return 1;
  }
}

void espera(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef)
{
  // Adiciona H ao fim da fila de espera de B
  lista_insere(base->espera, heroi->id, -1);

  // Cria e insere na LEF o evento AVISA (agora, B)
  cria_evento(lef, tempo, AVISA, NULL, base, NULL);
}

void desiste(unsigned int tempo, heroi *heroi, base *base_atual, struct fprio_t *lef, mundo *mundo_atual)
{
  // Escolhe uma base destino D aleatória diferente da atual
  unsigned int num_bases = mundo_atual->num_bases;
  unsigned int id_base_destino;

  do
  {
    id_base_destino = random_int(0, num_bases - 1);
  } while (id_base_destino == base_atual->id); // Evita a mesma base

  base *destino = &mundo_atual->bases[id_base_destino];

  // Cria e insere na LEF o evento VIAJA (agora, H, D)
  cria_evento(lef, tempo, VIAJA, heroi, destino, NULL);
}

void avisa(unsigned int tempo, base *base, struct fprio_t *lef, mundo *mundo_atual)
{
  // Enquanto houver vaga em B e houver heróis esperando na fila
  while (cjto_card(base->presentes) < (int)base->lotacao && lista_tamanho(base->espera) != 0)
  {
    // Retira o primeiro herói (H') da fila de B
    int id_heroi;
    lista_retira(base->espera, &id_heroi, 0);

    // Adiciona H' ao conjunto de heróis presentes em B
    cjto_insere(base->presentes, id_heroi);

    // Admissao
    printf("\n%6u: AVISA    PORTEIRO BASE %2d ADMITE %2d",
           tempo,
           base->id,
           id_heroi);

    // Encontra o ponteiro para o herói H' usando o id
    heroi *heroi_ptr = &mundo_atual->herois[id_heroi];

    // Cria e insere na LEF o evento ENTRA (agora, H', B)
    cria_evento(lef, tempo, ENTRA, heroi_ptr, base, NULL);
  }
}

int entra(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef)
{
  // Calculate TPB (tempo de permanência na base)
  int random_factor = random_int(1, 20);
  unsigned int tpb = 15 + heroi->paciencia * random_factor;

  cria_evento(lef, tempo + tpb, SAI, heroi, base, NULL);
  return tempo + tpb;
}

void sai(unsigned int tempo, heroi *heroi, base *base_atual, struct fprio_t *lef, mundo *mundo_atual)
{
  // Validate inputs
  if (!heroi || !base_atual || !lef || !mundo_atual)
  {
    fprintf(stderr, "Invalid NULL pointer in sai function\n");
    return;
  }

  if (cjto_retira(base_atual->presentes, heroi->id) < 0)
  {
    fprintf(stderr, "Failed to remove hero from base\n");
    return;
  }

  if (mundo_atual->num_bases <= 1)
  {
    morre(tempo, heroi, base_atual, lef, mundo_atual);
    return;
  }

  unsigned int tries = 0;
  unsigned int id_base_destino;
  do
  {
    id_base_destino = random_int(0, mundo_atual->num_bases - 1);
    tries++;
    if (tries > 100)
    {
      morre(tempo, heroi, base_atual, lef, mundo_atual);
      return;
    }
  } while (id_base_destino == base_atual->id);

  base *destino = &mundo_atual->bases[id_base_destino];

  cria_evento(lef, tempo, VIAJA, heroi, destino, NULL);

  cria_evento(lef, tempo, AVISA, NULL, base_atual, NULL);
}

void viaja(unsigned int tempo, heroi *heroi, base *base_destino, struct fprio_t *lef, mundo *mundo_atual)
{
  base *base_atual = &mundo_atual->bases[heroi->base];

  double distancia = calcular_distancia(base_destino->local, base_atual->local);

  // duracao
  unsigned int duracao = (unsigned int)(distancia / heroi->velocidade);

  cria_evento(lef, tempo + duracao, CHEGA, heroi, base_destino, NULL);
  printf("BASE %2u DIST %.0f VEL %d CHEGA %d", base_destino->id, distancia, heroi->velocidade, tempo + duracao);
}

void morre(unsigned int tempo, heroi *heroi, base *base, struct fprio_t *lef, mundo *mundo_atual)
{
  mundo_atual->herois_mortos++;
  cjto_retira(base->presentes, heroi->id);

  heroi->vivo = false;

  cria_evento(lef, tempo, AVISA, NULL, base, NULL);
}

void missao_ev(unsigned int tempo, missao *missao_atual, struct fprio_t *lef, mundo *mundo_atual)
{
  base *base_mais_proxima = NULL;
  struct cjto_t *habilidades_bmp = NULL;
  int menor_distancia = N_TAMANHO_MUNDO * N_TAMANHO_MUNDO;

  for (unsigned i = 0; i < mundo_atual->num_bases; i++)
  {
    base *base_atual = &mundo_atual->bases[i];

    struct cjto_t *habilidades_da_base = cjto_cria(N_HABILIDADES);
    if (!habilidades_da_base)
      continue;

    for (int j = 0; j < N_HEROIS; j++)
    {
      if (cjto_pertence(base_atual->presentes, j))
      {
        heroi *h = &mundo_atual->herois[j];
        struct cjto_t *temp = cjto_uniao(habilidades_da_base, h->habilidades);
        if (temp)
        {
          cjto_destroi(habilidades_da_base);
          habilidades_da_base = temp;
        }
      }
    }

    if (cjto_contem(habilidades_da_base, missao_atual->habilidades))
    {
      int distancia = calcular_distancia(missao_atual->local, base_atual->local);
      if (distancia < menor_distancia)
      {
        menor_distancia = distancia;
        base_mais_proxima = base_atual;

        if (habilidades_bmp)
          cjto_destroi(habilidades_bmp);
        habilidades_bmp = cjto_copia(habilidades_da_base);
      }
    }
    cjto_destroi(habilidades_da_base);
  }

  if (base_mais_proxima != NULL && habilidades_bmp != NULL)
  {
    mundo_atual->missoes_cumpridas++;
    missao_atual->cumprida = true;
    printf("\n%6d: MISSAO %d CUMPRIDA BASE %d HABS: [ ",
           tempo, missao_atual->id, base_mais_proxima->id);
    cjto_imprime(habilidades_bmp);
    printf(" ]");

    for (int k = 0; k < N_HEROIS; k++)
    {
      if (cjto_pertence(base_mais_proxima->presentes, k))
      {
        heroi *heroi_atual = &mundo_atual->herois[k];
        double risco = missao_atual->perigo /
                       (heroi_atual->paciencia + heroi_atual->experiencia + 1.0);

        if (risco > random_int(1, 30))
        {
          cria_evento(lef, tempo, MORRE, heroi_atual, base_mais_proxima, NULL);
        }
        else
        {
          heroi_atual->experiencia++;
        }
      }
    }
    cjto_destroi(habilidades_bmp);
  }
  else
  {
    missao_atual->tentativas++;
    printf("\n%6d: MISSAO %d IMPOSSIVEL", tempo, missao_atual->id);
    cria_evento(lef, tempo + 1440, MISSAO, NULL, NULL, missao_atual);
  }
}
