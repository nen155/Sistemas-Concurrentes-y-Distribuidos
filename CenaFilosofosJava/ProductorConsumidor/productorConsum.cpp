#include "mpi.h"
#include <iostream>
#include <math.h>

#define Buffer 5
#define ITERPRO 4
#define ITERCON 5

#define TAM 2

int contPro=0;
int contConsu=0;

using namespace std;

void productor(int rank)
{ 
	for( unsigned int i = 0 ; i < ITERPRO; i++)
	{ 
		cout << " Productor "<<rank<<" produce valor " << i << endl<<flush; 
		MPI_Ssend( &i, 1, MPI_INT, Buffer, 0, MPI_COMM_WORLD );//La etiqueta del productor es 0
		
		
	}
}
void consumidor(int rank)
{ 
		int value, peticion=1; 
		float raiz; 
		MPI_Status status;
		for( unsigned int i = 0 ; i < ITERCON ; i++ )
		{
			MPI_Ssend( &peticion, 1, MPI_INT, Buffer, 1, MPI_COMM_WORLD );//La etiqueta del consumidor es 1
			MPI_Recv ( &value, 1, MPI_INT, Buffer, 2, MPI_COMM_WORLD, &status);//La etiqueta del buffer es 2
			cout <<" Consumidor "<< rank<<" recibe valor "<<value<<" de Buffer "<<endl <<flush;
			raiz = sqrt(value);
		}
}

void buffer()
{ 
	int value[TAM], peticion, pos=0,rama;
	MPI_Status status;
	for( unsigned int i = 0 ; i < (ITERCON*ITERPRO)*2 ; i++ )
	{ 
			if (pos==0) 
				rama=0;// el consumidor no puede consumir
			else if (pos==TAM) 
				rama=1; // el productor no puede producir
			else // se puede consumir o producir
			{ 
				MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				rama= status.MPI_TAG;
			}
			switch(rama)
			{ 
				case 0 :
					MPI_Recv(&value[pos],1,MPI_INT,  MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);//Recibo de cualquier productor con etiqueta 0
					cout<< " Buffer recibe "<< value[pos] << " de Prod." << endl <<flush;
					pos++; 
				break;
				case 1 :
					MPI_Recv( &peticion,1, MPI_INT, MPI_ANY_SOURCE , 1, MPI_COMM_WORLD, &status);//Recibo peticion de cualquier consumidor con etiqueta 1
					MPI_Ssend( &value[pos-1], 1, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);//Envio al consumidor que me ha la petición el paso.
					cout<< " Buffer envia "<< value[pos-1] << " a Cons." << endl <<flush;
					pos--; 
				break;
			}
	}
}

int main( int argc, char *argv[] )
{
	int rank, size ;
	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank ) ;
	MPI_Comm_size( MPI_COMM_WORLD, &size ) ;
	if (rank >=0 && rank<=4) 
		productor(rank);//procesos 0 1 2 3 4 son del productor
    	else if (rank==5) 
		buffer();//proceso 5 es de gestion del buffer
    	else if(rank>=6 && rank<=9)
		consumidor(rank);//proceso 6 7 8 9 son del consumidor
	MPI_Finalize( ) ;
	return 0;
}
