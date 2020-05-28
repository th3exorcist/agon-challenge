/*
  
      █████╗  ██████╗  ██████╗ ███╗   ██╗
     ██╔══██╗██╔════╝ ██╔═══██╗████╗  ██║
     ███████║██║  ███╗██║   ██║██╔██╗ ██║
     ██╔══██║██║   ██║██║   ██║██║╚██╗██║
     ██║  ██║╚██████╔╝╚██████╔╝██║ ╚████║
     	╚═╝  ╚═╝ ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝
                                         

*/

#include <stdio.h> /* io */
#include <stdlib.h> /* ponteiro */
#include <unistd.h> /* sleep [o -Wall] */
#include <pthread.h> /* threads */
#include "agon_papiro.h" /* color */


/* estrutura de dados dos personagens */

typedef struct {
    int id; /* id */
    int num_ite; /* num_iterações */
} ArgP; 

/* variaveis globais */
int death_note = 0; /* 0 = ninguém escrevendo : 1 = alguém escrevendo */
pthread_mutex_t monitor = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t active = PTHREAD_MUTEX_INITIALIZER; /*monitor utilizado para contar quantidade de threads ativas*/
pthread_cond_t cond[6]; /* variável de condição com a quantidade de personagens */
int waiting_queue[6] = {0, 0, 0, 0, 0, 0}; /* contem os personagens esperando o death_note */
int write_death_note[6] = {0, 0, 0, 0, 0, 0}; /* conta quantas vezes a pessoa escreveu no death_note */
int queue = 0; /* contador de pessoas na fila */
int no_more_ite = 0; /* contador que indica quando acaba as interaçoes das threads */
int flag = 0; /* flag para identificar deadlock */
int prior = 3; /* veriavel usada pra comparar prioridades */

/* A função name retorna o nome do personagem, os nomes foram posicionados de forma que
   as duplas fiquem a 3 posições de distância, por exemplo [Ed Warren e Lorraine], [Kira e Misa e [Geralt e Yennefer] para definição de prioridades.  */

char *name (int id) {

    char *name;

    switch (id) {
        case 0 :
                name = "Ed Warren";
                break;
        case 1 :
                name = "Kira";
                break;
        case 2 :
                name = "Geralt";
                break;
        case 3 :
                name= "Lorraine";
                break;
        case 4 :
                name = "Misa";
                break;
        case 5 :
                name = "Yennefer";
                break;
        default :
                break;
    }

    return name;
}

/* Função de prioridade, recebe o argumento do tipo personagem, retorna qual personagem na fila deve escrever no death_note*/

void priority(ArgP *character){

	if (( waiting_queue[0] == 1 || waiting_queue[3] == 1 ) && ( waiting_queue[1] == 1 || waiting_queue[4] == 1 ) && ( waiting_queue[2] == 1 || waiting_queue[5] == 1 ) ){
		flag = 1;
	} 

	else {
		flag = 0; /* flag do deadlock */
		if ((*character).id  == 0 ){
			if (waiting_queue[1] == 1){
				prior = 1;
			}
			else if (waiting_queue[4] == 1){
				prior = 4;
			}
			else if (waiting_queue[2] == 1){
				prior = 2;
			}
			else if (waiting_queue[5] == 1){
				prior = 5;
			}
			else if (waiting_queue[3] == 1){
				prior = 3;
			}  			
		}
		else if ((*character).id  == 1 ){	
			if (waiting_queue[2] == 1){
				prior = 2;
			}
			else if (waiting_queue[5] == 1){
				prior = 5;
			
			}
			else if (waiting_queue[0] == 1){
				prior = 0;			
			}
			else if (waiting_queue[3] == 1){		
				prior = 3;
			}
			else if (waiting_queue[4] == 1){
				prior = 4;
			}
		}
		else if ((*character).id  == 2){
			if (waiting_queue[0] == 1){
				prior = 0;
			}
			else if (waiting_queue[3] == 1){
				prior = 3;
			}
			else if (waiting_queue[1] == 1){
				prior = 1;
			}
			else if (waiting_queue[4] == 1){
				prior = 4;
			}
			else if (waiting_queue[5] == 1){
				prior = 5;
			}
		}
		else if ((*character).id  == 3){
			if (waiting_queue[1] == 1){
				prior = 1;
			}
			else if (waiting_queue[4] == 1){
				prior = 4;
			}
			else if (waiting_queue[2] == 1){
				prior = 2;
			}
			else if (waiting_queue[5] == 1){
				prior = 5;
			}
			else if (waiting_queue[0] == 1){
				prior = 0;
			}
		}
		else if ((*character).id == 4){
			if (waiting_queue[2] == 1){
				prior = 2;
			}
			else if (waiting_queue[5] == 1){
				prior = 5;
			}
			else if (waiting_queue[0] == 1){
				prior = 0;
			}
			else if (waiting_queue[3] == 1){
				prior = 3;
			}
			else if (waiting_queue[1] == 1){
				prior = 1;
			}
		}
		else if ((*character).id  == 5){
			if (waiting_queue[0] == 1){
				prior = 0;
			}
			else if (waiting_queue[3] == 1){
				prior = 3;
			}
			else if (waiting_queue[1] == 1){
				prior = 1;
			}
			else if (waiting_queue[4] == 1){
				prior = 4;
			}
			else if (waiting_queue[2] == 1){
				prior = 2;
			}
		}		
	}
}

/* Função que apenas printa na tela que tem personagem esta escrevendo no death_note e o mesmo tem 3 segundos para escrever um nome. */

void writing_death_note(int id) {			
	write_death_note[id]++; /*incrementa no número de vezes que o personagem escreve no death_note*/ 
    printf("==================^=-=^======================\n");   		 
    printf("%s está escrevendo no death_note\n", name(id));
    printf("==================,===,======================\n"); 
    sleep(1);
}

/* Função que coloca o personagem um tempo ficar fazendo outra quest */

void winning_xp() {
    sleep(3 + rand() % 4); /* Entre 3 e 6 segundos */
}

/* Função onde o personagem vai requisitar o death_note */

void want_write_death_note(ArgP *character) { 
    pthread_mutex_lock(&monitor); /* mutex */
    	printf("%s Quer escrever no Death Note\n", name((*character).id));
      printf("\n");

        if ((death_note == 1) || (queue > 0)){ /* se o death_note está ocupado ou tem mais alguém na fila */
			waiting_queue[(*character).id] = 1; 
           	queue++; /* contador de pessoas na fila */      
           	pthread_cond_wait(&cond[(*character).id], &monitor); /* da um wait no id do personagem */            
        }

    death_note = 1; /* death_note ocupado...alguém está escrevendo  */
    pthread_mutex_unlock(&monitor); /* libera mutex */
}

/* função onde o personagem que estava utilizando o death_note o libera e escolhe um personagem da fila se for possível, olhando sua prioridade */

void wrote_a_name(ArgP *character){
    pthread_mutex_lock(&monitor);
        printf("%s Escreveu um nome no Death Note\n", name((*character).id));
        printf("\n");
        
        death_note = 0; /* abre o death_note */ 

        priority(character); /* verifica prioridade */        

        if (flag == 0){       /* flag do deadlock */
        	pthread_cond_signal(&(cond[prior])); /*sinaliza */
        	waiting_queue[prior] = 0;  /* fila */
        	queue--; /* decrementa o contador de pessoas na fila */
    	}

    pthread_mutex_unlock(&monitor); /* mutex */
}

/* personagens */

void *characters(void *argument) {

	ArgP *character = (ArgP *) argument; /* (ArgP *) é um cast para a variável genérica argument */
 
	pthread_mutex_lock(&active); /* mutex */
	no_more_ite++; /*incrementa o numero de threads ativas*/
	pthread_mutex_unlock(&active);	

    while ((*character).num_ite > 0) { /* enquanto houver iterações permitidas */
        want_write_death_note(character); /* mutex para pegar o death_note */
        writing_death_note((*character).id); 
        wrote_a_name(character); /* mutex para liberar o death_note */
        winning_xp(); /* função para os personagens passarem de 3 a 6 segundos fazendo outras coisas */
        (*character).num_ite--; /* decrementa o número de iterações que foi passado via parâmetro */      
    }

    pthread_mutex_lock(&active);
		no_more_ite--;/*decrementa o numero de threads ativas*/
	  pthread_mutex_unlock(&active);	

	pthread_exit(NULL);

}

/* Agon o espírito do concursos e das disputas intervem quando acha necessário. No caso ele detecta deadloack e muda a ordem de prioridade.  */
void *agon(){

	int found = 0; /* flag que encontrou um deadklock */
	int res = 0; /* auxiliar de iteração */
	int pos = 0; /* posição */

	while (no_more_ite > 0){ /* enquanto as threads não terminaram (acabou == 0 indica que terminaram) */
		sleep (5); /* faz o agon passar de 5 em 5 segundos checando */
		found = write_death_note[1]; /* achou recebe o primeiro */

		if ((death_note == 0) && (queue > 0)) { /* se o death_note estiver aberto e se estiver pessoas esperando na fila */
			puts ("Agon detectou um deadlock e intervem.");
      printf("\n");
			for (res = 0; res < 6;res++){ /*agon quando há deadlock libera o personagem que menos write o death_note, trata a inanição*/
				if (write_death_note[res] <= found){
					found = write_death_note[res];
					pos = res;
				}
			}

			printf ("Liberando...[Blesses] %s \n", name(pos));
      printf("\n");
			pthread_cond_signal(&(cond[pos])); /* sinaliza */
			waiting_queue[pos] = 0;
			queue--; /* decrementa o contador de pessoas da fila */
		}
	}

	pthread_exit(NULL);
}

/* Cria as threads dos personagens */

void create_threads_characters(pthread_t threads[], ArgP argument_threads[]) {

    int j = 0;

    while (j < 6) {
        /* personagens é o ponteiro para execução de uma função, e argumentos_threads[j] é o argumento passado referente a thread */
        pthread_create(&(threads[j]), NULL, characters, &argument_threads[j]);
        j++; 
    }
}
/* cria a thread do Agon */

void create_thread_agon(pthread_t thread){
	pthread_create(&(thread), NULL, agon, NULL);
}


/* Função principal */

int main(int argc, char **argv){


    int num_ite, j = 0;

    /* Criando as threads para os 6 personagens + uma separada para o Agon */

    pthread_t threads_characters[6];
    pthread_t thread_agon = 0;

    ArgP argumentos_threads[6];


     drawAgon();

    /* Argumento passado via linha de comando */

    if (argc == 2) {
        num_ite = atoi(argv[1]);
    }
    
    else {
        printf("\n\nVocê não passou o número de iterações via parâmetro, ou passou mais parâmetros do que o esperado.\n\n");
        exit(1);
    }

    for (j = 0; j < 6; j++) {
        argumentos_threads[j].id = j;  /* Atribuindo um id as pessoas */  
        argumentos_threads[j].num_ite = num_ite; /* Atribuindo o número de iterações as pessoas */    
    }

    /* Start as threads dos personagens */

    create_threads_characters(threads_characters, argumentos_threads);

    /* Start thread Agon */

    create_thread_agon(thread_agon);

    /* Esperando as threads terminarem */

    for (j = 0; j < 6; j++) {
        pthread_join(threads_characters[j], NULL);
    }
   
  	pthread_join(thread_agon,NULL);
    
    return 0;
}
