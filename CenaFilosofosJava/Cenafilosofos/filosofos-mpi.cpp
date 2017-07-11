#include "mpi.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


using namespace std;

void Filosofo( int id, int nprocesos )
{
	int value=1,suelta=0;
	int izq = (id+1) % nprocesos , der = (id+nprocesos-1) % nprocesos ;
	while ( true )
	{
		//Hago que el 0 o el 3 de los filosofos coja el tenedor derecho primero.
		if(id==0 || id==3){
			// solicita tenedor derecho
			cout << "Filosofo " << id << " coge tenedor der. " << der << endl;
			MPI_Ssend( &value, 1, MPI_INT, der,0, MPI_COMM_WORLD);
			cout << "Filosofo " << id << " COMIENDO" << endl ;
			sleep( (rand()%3)+1 ); // comiendo
			// solicita tenedor izquierdo
			cout << "Filosofo " << id << " solicita tenedor izq. " << izq << endl;
			MPI_Ssend( &value, 1, MPI_INT, izq,0, MPI_COMM_WORLD);
		//Sino son esos dos filosofos los demás cogen el izquierdo primero así se evita el interbloqueo
		}
		else
		{
			// solicita tenedor izquierdo
			cout << "Filosofo " << id << " solicita tenedor izq. " << izq << endl;
			MPI_Ssend( &value, 1, MPI_INT, izq,0, MPI_COMM_WORLD);
			// solicita tenedor derecho
			cout << "Filosofo " << id << " coge tenedor der. " << der << endl;
			MPI_Ssend( &value, 1, MPI_INT, der,0, MPI_COMM_WORLD);
		}
		
		cout << "Filosofo " << id << " COMIENDO" << endl ;
		sleep( (rand()%3)+1 ); // comiendo

		// suelta el tenedor izquierdo
		cout << "Filosofo "<<id<< " suelta tenedor izq. " << izq << endl;
		MPI_Ssend( &suelta, 1, MPI_INT, izq,1, MPI_COMM_WORLD);

		// suelta el tenedor derecho
		cout << "Filosofo " << id << " suelta tenedor der. " << der << endl;
		MPI_Ssend( &suelta, 1, MPI_INT, der,1, MPI_COMM_WORLD);

		cout << "Filosofo " << id << " PENSANDO" << endl;
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
		 MPI_Recv( &buf, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,&status);

		// Recibe la peticion del filosofo ...
		Filo=status.MPI_SOURCE;
		cout << "Ten. " << id << " recibe petic. de " << Filo << endl;

		// Espera a que el filosofo suelte el tenedor...
		MPI_Recv( &buf,1, MPI_INT, Filo , 1, MPI_COMM_WORLD, &status);
		cout << "Ten. " << id << " recibe liberac. de " << Filo << endl ;
	}
}

int main( int argc, char ** argv )
{ 

	int rank,size;
	srand( time(0) );
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	if ( size != 10 )
	{ 
		if (rank == 0) 
			cout << "El numero de procesos debe ser 10" << endl;
		MPI_Finalize( ); return 0;
	}
	if ( rank%2 == 0) 
		Filosofo(rank,size); // los pares son filosofos
	else  
		Tenedor(rank,size);
	// los impares son tenedores
	MPI_Finalize();
	return 0;
}
