#include <time.h>
#include <unistd.h> // incluye ”usleep(...)”
#include <stdlib.h> // incluye ”rand(...)” y ”srand”
#include <pthread.h>
#include <iostream>
#include <semaphore.h>

using namespace std;
//Semaforos para cada uno de los fumadores y el estanquero
sem_t fumador[3];
sem_t estanq;
// funcion que simula la accion de fumar
// como un retardo aleatorio de la hebra
void fumar()
{ // calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
	const unsigned miliseg = 100U + (rand() % 1900U) ;
	usleep( 1000U*miliseg ); // retraso bloqueado durante miliseg milisegundos
	
}
int  ingredientes(){
	return (int)rand()%3;
}
void * estanquero(void *){
	//Se establece un bucle infinito de produccion de materiales
	while(true){
		//El estanquero no puede volver a producir un material hasta que
		//uno de los fumadores no lo haya cogido
		sem_wait(&estanq);
		int ingr = ingredientes();
		cout<<"produciendo"<< ingr+1<<endl;
		//Dependiendo del tipo de ingrediente producido
		//da paso a un fumador u otro para que coja ese ingrediente.
		sem_post(&fumador[ingr]);
	}
	
	
}
void * fumando(void *i){
	//Los fumadores fuman continuamente
	while(true){
			//Se da paso al fumador cuyo ingrediente esta producido
		sem_wait(&fumador[(unsigned long)i]);
		//Inmediatamente se le da paso al estanquero a producir
		sem_post(&estanq);
		cout<<"fumando"<<(unsigned long)i+1<<endl;
		fumar();
	}
}
int main()
{
	srand( time(NULL) ); // inicializa la semilla aleatoria
	//Ningun fumador puede fumar hasta que no produzca algo el estanquero
	sem_init(&fumador[0],0,0);
	sem_init(&fumador[1],0,0);
	sem_init(&fumador[2],0,0);
	//El estanquero puede producir mientras fuman los fumadores, ya sea 1 o varios,
	//los que fuman. La acción de fumar requiere un tiempo
	//el estanquero solo esta parado mientras coge el material el fumador correspondiente.
	sem_init(&estanq,0,1);
	pthread_t fumadores[3];
	pthread_t estanque;
		for (int i = 0; i < 3; i++)
			pthread_create(&fumadores[i], NULL, fumando, (void *)i);
		pthread_create(&estanque, NULL, estanquero, NULL);

	for(int i=0;i<3;i++)
		pthread_join(fumadores[i],NULL);
	pthread_join(estanque, NULL);
	pthread_exit(NULL);
}
