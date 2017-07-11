#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

const int  num_items = 30;
const int  tam_vec =10;
int primera_libre=0;
int buffer[tam_vec];
sem_t puede_consumir;
sem_t puede_producir;

int producir_dato()
{
	static int contador = 1 ;
	return contador ++ ;
}
void consumir_dato( int dato )
{
	cout << "dato recibido: " << dato << endl ;
}

void * productor( void * )
{ 
	
	for( unsigned i = 0 ; i < num_items ; i++ )
	{ 
			//Puede producir datos mientras que se este consumiendo alguno
			int dato = producir_dato();
			//Al tener el semaforo puede_producir el tamaño del vector,
			//permite rellenar el vector entero si asi le dejase el procesador,
			//es decir permite la produccion de mas de un elemento
			sem_wait(&puede_producir);
			buffer[primera_libre++] = dato;
			sem_post(&puede_consumir);		
		
	}
	
	return NULL ;
}
void * consumidor( void * )
{ 
	
	for( unsigned i = 0 ; i < num_items ; i++ )
	{ 
			
			int dato ;
			//Espera hasta que por lo menos haya producido un dato,
			//ya que su semaforo puede_consumir esta a 0 no entrara a menos que se haya producido
			sem_wait(&puede_consumir);
			dato = buffer[--primera_libre];
			sem_post(&puede_producir);
			consumir_dato( dato ) ;
	}
	
	return NULL ;
}
int main(){
	pthread_t product;
	pthread_t consum;
	sem_init(&puede_producir, 0, tam_vec);
	sem_init(&puede_consumir, 0, 0);
	
	pthread_create( &(product), NULL,productor , NULL );
	pthread_create( &(consum), NULL,consumidor , NULL );
	pthread_join( product, NULL );
	pthread_join( consum, NULL );
	sem_destroy(&puede_consumir);
	sem_destroy(&puede_producir);
	cout<<"fin"<<endl;
	pthread_exit(NULL);
	
	
}
