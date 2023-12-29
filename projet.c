#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define N1 4
#define M1 5
#define N2 5
#define M2 3

int B[N1][M1]; 
int C[N2][M2];
int A[N1][M2];

int T[10]; 

pthread_mutex_t mutex;

// Fonction thread producteur 
void* produit(void* arg) {
  int num_ligne = *(int*)arg;
  
  for(int j=0; j<M2; j++) {
    int val = 0;
    for(int k=0; k<M1; k++) {
      val += B[num_ligne][k] * C[k][j]; 
    }
    
    pthread_mutex_lock(&mutex);
    T[num_ligne*M2 + j] = val;
    pthread_mutex_unlock(&mutex);
  }
  
  printf("Producteur %d termine\n", num_ligne);
  pthread_exit(NULL);
}

// Fonction thread consommateur
void* consomme(void* arg) {
  int num_ligne = *(int*)arg;

  for(int j=0; j<M2; j++) {
    pthread_mutex_lock(&mutex);
    int val = T[num_ligne*M2 + j]; 
    pthread_mutex_unlock(&mutex);
    
    A[num_ligne][j] = val;
  }

  printf("Consommateur %d termine\n", num_ligne);
  pthread_exit(NULL); 
}

int main() {

  // Initialisation des matrices
  for(int i=0; i<N1; i++) {
    for(int j=0; j<M1; j++) {
      B[i][j] = rand() % 10; 
    }
  }

  for(int i=0; i<N2; i++) {
    for(int j=0; j<M2; j++) {
      C[i][j] = rand() % 10;
    }
  }

  // Initialisation du mutex
  pthread_mutex_init(&mutex, NULL);
  
  // Création des threads
  pthread_t prod[N1];
  pthread_t cons[N1];
  int num_lignes[N1];

  for(int i=0; i<N1; i++) {
    num_lignes[i] = i;
    pthread_create(&prod[i], NULL, produit, (void*) &num_lignes[i]);
    pthread_create(&cons[i], NULL, consomme, (void*) &num_lignes[i]); 
  }

  // Attendre la fin des threads
  for(int i=0; i<N1; i++) {
    pthread_join(prod[i], NULL);
    pthread_join(cons[i], NULL); 
  }

  // Afficher la matrice resultante
  for(int i=0; i<N1; i++) {
    for(int j=0; j<M2; j++) {
      printf("%d ", A[i][j]); 
    }
    printf("\n");
  }

  pthread_mutex_destroy(&mutex);

  return 0;
}
