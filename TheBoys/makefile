# Compilador e flags de compilação
CC = gcc
CFLAGS = -Wall -Werror -Wextra -std=c99 -g

# Lista de arquivos objeto
OBJS = theboys.o oscaras.o fprio.o lista.o conjunto.o

# Alvo padrão
all: theboys

# Regras de linkagem
theboys: $(OBJS)
	$(CC) $(CFLAGS) -o theboys $(OBJS)

# Compilação dos módulos
theboys.o: theboys.c oscaras.h fprio.h lista.h conjunto.h
	$(CC) $(CFLAGS) -c theboys.c

oscaras.o: oscaras.c oscaras.h fprio.h lista.h conjunto.h
	$(CC) $(CFLAGS) -c oscaras.c

fprio.o: fprio.c fprio.h lista.h
	$(CC) $(CFLAGS) -c fprio.c

lista.o: lista.c lista.h
	$(CC) $(CFLAGS) -c lista.c

# Limpeza dos arquivos compilados
clean:
	rm -f theboys.o oscaras.o fprio.o lista.o theboys
