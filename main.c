#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t rw_mutex;      // Semáforo para controle de escritores
sem_t mutex;         // Semáforo para proteger o contador de leitores
int read_count = 0;  // Contador de leitores ativos
int newspaper = 0;   // Jornal compartilhado (recurso)

void* reader(void* arg) {
    int reader_id = *(int*)arg;
    free(arg);

    sem_wait(&mutex);
    read_count++;
    if (read_count == 1) {
        sem_wait(&rw_mutex); // Bloqueia escritores enquanto há leitores
    }
    sem_post(&mutex);

    // Seção de leitura
    printf("📖 Leitor %d entrou no café e leu o jornal: %d\n", reader_id, newspaper);
    sleep(1); // Simula tempo de leitura

    sem_wait(&mutex);
    read_count--;
    if (read_count == 0) {
        sem_post(&rw_mutex); // Último leitor libera escritores
    }
    sem_post(&mutex);

    pthread_exit(NULL);
}

void* writer(void* arg) {
    int writer_id = *(int*)arg;
    free(arg);

    sem_wait(&rw_mutex); // Escritor precisa de exclusividade
    newspaper++;  // Modifica o jornal compartilhado
    printf("✍️ Escritor %d entrou no café e escreveu: %d\n", writer_id, newspaper);
    sleep(1); // Simula tempo de escrita
    sem_post(&rw_mutex);

    pthread_exit(NULL);
}

int main() {
    int i;
    pthread_t readers[5], writers[5];

    // Inicializa os semáforos
    sem_init(&rw_mutex, 0, 1);
    sem_init(&mutex, 0, 1);

    // Cria threads para leitores e escritores
    for (i = 0; i < 5; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&readers[i], NULL, reader, id);

        id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&writers[i], NULL, writer, id);
    }

    // Espera todas as threads terminarem
    for (i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
        pthread_join(writers[i], NULL);
    }

    // Destroi os semáforos
    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);

    return 0;
}
