#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;

void Filosofo( int id, int nprocesos )
{
	MPI_Status status;
	int value=1,suelta=0;
	int izq = (id+1) % nprocesos , der = (id+nprocesos-1) % nprocesos ;
	while ( true )
	{
		// solicita sentarse
		cout << "Filosofo " << id << " solicita sentarse " << endl<<flush;
		MPI_Ssend( &value, 1, MPI_INT, 10,2, MPI_COMM_WORLD);//Solicita al camarero 10 sentarse
		MPI_Recv( &value, 1, MPI_INT, 10,2, MPI_COMM_WORLD,&status);//Espera respuesta del camarero 10


		// solicita tenedor izquierdo
		cout << "Filosofo " << id << " solicita tenedor izq. " << izq << endl<<flush;
		MPI_Ssend( &value, 1, MPI_INT, izq,1, MPI_COMM_WORLD);
		// solicita tenedor derecho
		cout << "Filosofo " << id << " coge tenedor der. " << der << endl<<flush;
		MPI_Ssend( &value, 1, MPI_INT, der,1, MPI_COMM_WORLD);
		
		
		cout << "Filosofo " << id << " COMIENDO" << endl ;
		sleep( (rand()%3)+1 ); // comiendo

		// suelta el tenedor izquierdo
		cout << "Filosofo "<<id<< " suelta tenedor izq. " << izq << endl<<flush;
		MPI_Ssend( &suelta, 1, MPI_INT, izq,0, MPI_COMM_WORLD);

		// suelta el tenedor derecho
		cout << "Filosofo " << id << " suelta tenedor der. " << der << endl<<flush;
		MPI_Ssend( &suelta, 1, MPI_INT, der,0, MPI_COMM_WORLD);
		

		//filosofo se levanta
	        cout<<"Filosofo "<<id<< " se levanta"<<endl<<flush;
	        MPI_Ssend( &value, 1, MPI_INT,  10, 3, MPI_COMM_WORLD);//Le dice al camarero que quiere levantarse


		cout << "Filosofo " << id << " PENSANDO" << endl<<flush;
		sleep( (rand()%3)+1 ); // pensando
	}
}
void Tenedor( int id, int nprocesos )
{
	int buf, Filo ;
	MPI_Status status;
	while ( true )
	{ 
		// Espera un peticion desde cualquier filosofo vecino ...
		 MPI_Recv( &buf, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,&status);

		// Recibe la peticion del filosofo ...
		Filo=status.MPI_SOURCE;
		cout << "Ten. " << id << " recibe petic. de " << Filo << endl<<flush;

		// Espera a que el filosofo suelte el tenedor...
		MPI_Recv( &buf,1, MPI_INT, Filo , 0, MPI_COMM_WORLD, &status);
		cout << "Ten. " << id << " recibe liberac. de " << Filo << endl <<flush;
	}
}
void Camarero(){
	int cont=0,filo,value;
	MPI_Status status;

	while(1){
		if(cont<4)//Si todavía no se han sentado 4 filosofos les permito sentarse o levantarse
			MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);//puede sentarse o levantarse
        	else
            		MPI_Probe(MPI_ANY_SOURCE,3, MPI_COMM_WORLD, &status);//solo puede levantarse

		if (status.MPI_TAG==2) {//Etiqueta 2 sentarse
	       	    filo=status.MPI_SOURCE;//Obtengo la id del filosofo que envia la peticion
		    MPI_Recv( &value, 1, MPI_INT, filo, 2, MPI_COMM_WORLD,&status);//Recibo la peticion de sentarse
		    cont++; //Un filosofo mas sentado
		    MPI_Ssend( &value, 1, MPI_INT, filo, 2, MPI_COMM_WORLD); //Le digo que puede sentarse
		    cout<< "Filosofo "<< filo << " se sienta. Hay "<<cont<<" filosofos sentados. "<<endl<<flush;

		}
		if (status.MPI_TAG==3) {//Etiqueta 3 se sienta.
		    filo=status.MPI_SOURCE;//Obtengo el id del filosofo que envia la peticion
		    MPI_Recv( &value, 1, MPI_INT, filo, 3, MPI_COMM_WORLD,&status);//Recibo la peticion de levantarse, no tengo que responderle pues levantarse solo libera el recurso cont para que entre otro a la mesa
		    cont--; // Un filosofo se levanta
		    cout<< "Filosofo "<< filo << " se levanta. Hay "<<cont<<" filosofos sentados. "<<endl<<flush;

		}		
		
	}
}

int main( int argc, char ** argv )
{ 

	int rank,size,numprocess=10;//numprocess es 10 porque no cuento el camarero para hacer los calculos de pares e impares en tenedores y filosofos.
	srand( time(0) );
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	if ( size != 11 )//Hay 11 procesos en total contando al camarero.
	{ 
		if (rank == 0) 
			cout << "El numero de procesos debe ser 11" << endl;
		MPI_Finalize( ); return 0;
	}
	if ( rank%2 == 0 && rank!=10) //Cojo los filosofos que son los pares menos el camarero que es el 10
		Filosofo(rank,numprocess); // los pares son filosofos
	else if(rank!=10)//Los impares menos el camarero son los tenedores
		Tenedor(rank,numprocess);
	// los impares son tenedores
	else
	if(rank==10)//Tenemos al camarero
		Camarero();
	MPI_Finalize();
	return 0;
}
