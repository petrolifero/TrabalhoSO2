#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define CHAIRS 5                /* número de cadeiras para os clientes à espera */
#define BARBERS 4

sem_t customers[BARBERS];                /* número de cliente à espera de atendimento */
sem_t barbers[BARBERS];         /* vetor de barbeiros à espera de clientes */
sem_t mutex;                    /* para exclusão mútua */
int waiting = 0;                /* clientes que estão esperando (não estão cortando) */

/* protótipos */
void* barber(void *arg);
void* customer(void *arg);
void cut_hair();
void customer_arrived();
void get_haircut();
void giveup_haircut();

int main() {
int i;
srand(time(NULL));
for(i=0; i<BARBERS; i++)
{
	sem_init(&customers[i], 1, 0);
}
for(i=0; i<BARBERS; i++)
	sem_init(&barbers[i], 1, 0);
sem_init(&mutex, 1, 1);

pthread_t b[BARBERS], c;

 /* criando BARBERS barbeiros */
for(i=0; i<BARBERS; i++)
{
	int j=i;
	pthread_create(&b[i], NULL, (void *) barber, (void*)&j);
	sleep(1);
}

 /* criação indefinida de clientes */
while(1) {
	pthread_create(&c, NULL, (void *) customer, NULL);
	sleep(1);
}

return 0;
}

void* barber(void *arg) {
int i = *((int*)arg);
while(1) {
sem_wait(&customers[i]);   /* vai dormir se o número de clientes for 0 */
sem_wait(&mutex);       /* obtém acesso a 'waiting' */
waiting = waiting - 1;  /*descresce de um o contador de clientes à espera */
sem_post(&barbers[i]);     /* um barbeiro está agora pronto para cortar cabelo */
sem_post(&mutex);       /* libera 'waiting' */
cut_hair(i);             /* corta o cabelo (fora da região crítica) */
}

pthread_exit(NULL);
}

void* customer(void *arg) {
int whoINeed = rand()%BARBERS;
sem_wait(&mutex);           /* entra na região crítica */
if(waiting < CHAIRS) {      /* se não houver cadeiras vazias, saia */
customer_arrived();
waiting = waiting + 1;  /* incrementa o contador de clientes à espera */
sem_post(&customers[whoINeed]);   /* acorda o barbeiro se necessário */
sem_post(&mutex);       /* libera o acesso a 'waiting' */
sem_wait(&barbers[whoINeed]);     /* vai dormir se o número de barbeiros livres for 0 */
get_haircut();          /* sentado e sendo servido */
} else {
sem_post(&mutex);       /* a barbearia está cheia; não espera */
giveup_haircut();

}

pthread_exit(NULL);
}

void cut_hair(int id) {
printf("Barbeiro %d estah cortando o cabelo de alguem!\n",id);
sleep(3);
}

void customer_arrived() {
printf("Cliente chegou para cortar cabelo!\n");
}
void get_haircut() {
printf("Cliente estah tendo o cabelo cortado!\n");
}

void giveup_haircut() {
printf("Cliente desistiu! (O salao estah muito cheio!)\n");
}
