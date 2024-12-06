#include <stdio.h>
#include <stdlib.h>
#include "oscaras.h"
#include "fprio.h"
#include "time.h"

int main()
{
    srand(time(NULL));
    // Inicializa o mundo
    mundo mundo_atual;
    mundo_atual.num_herois = N_HEROIS;
    mundo_atual.num_bases = N_BASES;
    mundo_atual.num_missoes = N_MISSOES;
    mundo_atual.num_habilidades = N_HABILIDADES;
    mundo_atual.tamanho.x = N_TAMANHO_MUNDO;
    mundo_atual.tamanho.y = N_TAMANHO_MUNDO;
    mundo_atual.relogio = T_INICIO;

    // Aloca memória para heróis, bases e missões
    mundo_atual.herois = malloc(N_HEROIS * sizeof(heroi));
    mundo_atual.bases = malloc(N_BASES * sizeof(base));
    mundo_atual.missoes = malloc(N_MISSOES * sizeof(missao));
    int n_eventos = 0;

    if (!mundo_atual.herois || !mundo_atual.bases || !mundo_atual.missoes)
    {
        fprintf(stderr, "Erro ao alocar memória para o mundo\n");
        return 1;
    }

    // Inicializa a LEF (Lista de Eventos Futuros)
    struct fprio_t *lef = fprio_cria();
    if (!lef)
    {
        fprintf(stderr, "Erro ao criar a LEF\n");
        return 1;
    }

    // Inicializa o mundo e eventos iniciais
    inicializa_mundo(&mundo_atual, lef);
    fprio_imprime(lef);
    // Processa os eventos na LEF
    int evento_tipo;
    int evento_tempo;
    evento *ev;
    while (lef != NULL && ((ev = (evento *)fprio_retira(lef, &evento_tipo, &evento_tempo)) != NULL))
    {
        mundo_atual.relogio = ev->tempo;
        n_eventos++;
        int temp;
        // Processar de acordo com tipo
        switch (ev->tipo)
        {
        case CHEGA:
            if (ev->heroi != NULL && ev->heroi->vivo == true)
            {
                temp = chega(ev->tempo, ev->heroi, ev->base, lef);
                if (temp == 0)
                {
                    printf("\n%6u: CHEGA    HEROI %2u BASE %2u (%2d/%2d)",
                           ev->tempo, ev->heroi->id, ev->base->id, ev->base->presentes->num, ev->base->presentes->cap);
                }
                else
                {
                    printf("\n%6u: CHEGA    HEROI %2u BASE %u",
                           ev->tempo, ev->heroi->id, ev->base->id);
                }
            }
            break;

        case ESPERA:
            if (ev->heroi != NULL && ev->heroi->vivo == true)
            {
                espera(ev->tempo, ev->heroi, ev->base, lef);
                printf("\n%6u: ESPERA   HEROI %2u BASE %2u (%2d)",
                       ev->tempo, ev->heroi->id, ev->base->id, (ev->base->espera->tamanho - 1));
            }
            break;

        case DESISTE:
            if (ev->heroi != NULL && ev->heroi->vivo == true)
            {
                printf("\n%6u: DESISTE  HEROI %2u BASE %u",
                       ev->tempo, ev->heroi->id, ev->base->id);
                desiste(ev->tempo, ev->heroi, ev->base, lef, &mundo_atual);
            }
            break;

        case AVISA:
            if (ev != NULL)
            {
                printf("\n%6u: AVISA    PORTEIRO BASE %2u (%2d/%2d) FILA [ ",
                       ev->tempo, ev->base->id, ev->base->presentes->num, ev->base->lotacao);
                lista_imprime(ev->base->espera);
                printf(" ]");
                avisa(ev->tempo, ev->base, lef, &mundo_atual);
            }
            break;

        case ENTRA:
            if (ev->heroi != NULL && ev->heroi->vivo == true)
            {
                temp = entra(ev->tempo, ev->heroi, ev->base, lef);
                printf("\n%6u: ENTRA    HEROI %2u BASE %2u (%2d/%2d) SAI %d",
                       ev->tempo, ev->heroi->id, ev->base->id, ev->base->presentes->num, ev->base->presentes->cap, temp);
            }
            break;

        case SAI:
            if (ev->heroi != NULL && ev->heroi->vivo == true)
            {
                printf("\n%6u: SAI      HEROI %2u BASE %2u (%2d/%2d)",
                       ev->tempo, ev->heroi->id, ev->base->id, ev->base->presentes->num, ev->base->presentes->cap);
                sai(ev->tempo, ev->heroi, ev->base, lef, &mundo_atual);
            }
            break;

        case VIAJA:
            if (ev->heroi != NULL && ev->heroi->vivo == true)
            {
                printf("\n%6u: VIAJA    HEROI %2u BASE %2u ",
                       ev->tempo, ev->heroi->id, ev->base->id);
                viaja(ev->tempo, ev->heroi, ev->base, lef, &mundo_atual);
            }
            break;

        case MORRE:
            if (ev->heroi != NULL && ev->heroi->vivo == true)
            {
                morre(ev->tempo, ev->heroi, ev->base, lef, &mundo_atual);
                printf("\n%6u: MORRE    HEROI %2u BASE %u",
                       ev->tempo, ev->heroi->id, ev->base->id);
            }
            break;

        case MISSAO:
            if (ev != NULL)
            {
                printf("\n%6d: MISSAO   %-4d TENT %d HAB REQ: [ ", ev->tempo, ev->missao->id, ev->missao->tentativas);
                cjto_imprime(ev->missao->habilidades);
                printf(" ]");
                missao_ev(ev->tempo, ev->missao, lef, &mundo_atual);
                printf("\n%6u: MISSAO   %-4d LOCAL %u,%u",
                       ev->tempo, ev->missao->id,
                       ev->missao->local.x, ev->missao->local.y);
            }
            break;

        case FIM:
            printf("\n%6u: FIM", ev->tempo);
            fprio_destroi(lef);
            lef = NULL;
            break;

        default:
            fprintf(stderr, "\nEvento desconhecido: %d", ev->tipo);
            break;
        }

        free(ev);
    }

    int tentativas_totais = 0;
    int tentativa_maxima = 0;
    int tentativa_minima = -1;

    for (int h = 0; h < (int)mundo_atual.num_missoes; h++)
    {
        missao missao_atual = mundo_atual.missoes[h];
        tentativas_totais += missao_atual.tentativas;
        if (missao_atual.tentativas > tentativa_maxima)
        {
            tentativa_maxima = missao_atual.tentativas;
        }
        if (missao_atual.tentativas < tentativa_minima || tentativa_minima == -1)
        {
            tentativa_minima = missao_atual.tentativas;
        }
    }
    for (unsigned j = 0; j < mundo_atual.num_herois; j++)
    {
        printf("\nHEROI %2d: %b HABILIDADES: [", mundo_atual.herois[j].id, mundo_atual.herois[j].vivo);
        cjto_imprime(mundo_atual.herois[j].habilidades);
        printf(" ]");
    }
    for (unsigned j = 0; j < mundo_atual.num_bases; j++)
    {
        printf("\nBASE %2d: MISSOES CUMPRIDAS: %4d (%2d/%2d) [", mundo_atual.bases[j].id, mundo_atual.bases[j].missoes_completas, mundo_atual.bases[j].presentes->num, mundo_atual.bases[j].presentes->cap);
        cjto_imprime(mundo_atual.bases[j].presentes);
        printf(" ]");
    }

    printf("\nEVENTOS TRATADOS: %d", n_eventos);
    printf("\nMISSOES CUMPRIDAS: %4d/%4d %.2f\%%", mundo_atual.missoes_cumpridas, mundo_atual.num_missoes, (double)mundo_atual.missoes_cumpridas / (double)mundo_atual.num_missoes * 100);
    double taxa_de_mortalidade = (double)mundo_atual.herois_mortos * 100 / (double)mundo_atual.num_herois;
    printf("\nTENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f", tentativa_minima, tentativa_maxima, ((double)tentativas_totais / (double)mundo_atual.num_missoes));
    printf("\nTAXA DE MORTALIDADE: %.1f\%%", taxa_de_mortalidade);

    // Limpa a LEF e libera memória
    for (unsigned int i = 0; i < mundo_atual.num_herois; i++)
    {
        cjto_destroi(mundo_atual.herois[i].habilidades);
    }
    free(mundo_atual.herois);
    for (unsigned int i = 0; i < mundo_atual.num_bases; i++)
    {
        lista_destroi(mundo_atual.bases[i].espera);
        cjto_destroi(mundo_atual.bases[i].presentes);
    }
    free(mundo_atual.bases);
    for (unsigned int i = 0; i < mundo_atual.num_missoes; i++)
    {
        cjto_destroi(mundo_atual.missoes[i].habilidades);
    }
    free(mundo_atual.missoes);

    return 0;
}
