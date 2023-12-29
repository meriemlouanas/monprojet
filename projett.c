#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 4 // Taille des matrices

int B[N][N];
int C[N][N];
int A[N][N];
int tampon[N][N]; // Tampon pour stocker les résultats intermédiaires
int prochain = 0; // Indice du prochain élément à produire
int elements_restants = N; // Nombre d'éléments restants à consommer

sem_t plein; // Semaphore pour le tampon plein
sem_t vide; // Semaphore pour le tampon vide
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *producteur(void *arg) {
    while (1) {
        sem_wait(&vide);
        pthread_mutex_lock(&mutex);
        
        if (prochain >= N) {
            pthread_mutex_unlock(&mutex);
            sem_post(&plein);
            pthread_exit(NULL);
        }
        
        // Calculer une ligne de la matrice résultante
        int ligne = prochain;
        prochain++;
        
        pthread_mutex_unlock(&mutex);

        for (int j = 0; j < N; j++) {
            int resultat = 0;
            for (int k = 0; k < N; k++) {
                resultat += B[ligne][k] * C[k][j];
            }
            tampon[ligne][j] = resultat;
        }

        sem_post(&plein);
    }
}

void *consommateur(void *arg) {
    while (1) {
        sem_wait(&plein);
        pthread_mutex_lock(&mutex);
        
        if (elements_restants == 0) {
            pthread_mutex_unlock(&mutex);
            sem_post(&vide);
            pthread_exit(NULL);
        }
        
        elements_restants--;

        // Copier les valeurs du tampon dans la matrice résultante A
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = tampon[i][j];
            }
        }

        pthread_mutex_unlock(&mutex);
        sem_post(&vide);
    }
}

int main() {
    pthread_t threads_prod[N];
    pthread_t thread_cons;

    sem_init(&plein, 0, 0);
    sem_init(&vide, 0, N);

    // Initialisation et affichage de la matrice B
    printf("Matrice B :\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B[i][j] = i + j;
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    // Initialisation et affichage de la matrice C
    printf("Matrice C :\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = i - j;
            printf("%d ", C[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    // Création des threads producteurs
    for (int i = 0; i < N; i++) {
        pthread_create(&threads_prod[i], NULL, producteur, NULL);
    }

    // Création du thread consommateur
    pthread_create(&thread_cons, NULL, consommateur, NULL);

    // Attente de la terminaison des threads
    for (int i = 0; i < N; i++) {
        pthread_join(threads_prod[i], NULL);
    }
    pthread_join(thread_cons, NULL);

    // Affichage de la matrice résultante A
    printf("Matrice résultante A :\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }

    return 0;
}
