#include <iostream>
#include <pthread.h>
using namespace std ;
const unsigned num_hebras = 10 ;

unsigned long func( unsigned long n ) { 

return 4+n;
 }
void* proc( void* i ) { 
	return (void *) func( (unsigned long)(i) ) ;
 }

int main()
{
	pthread_t id_hebra[num_hebras] ;
	for( unsigned i = 0 ; i < num_hebras ; i++ )
		pthread_create( &(id_hebra[i]), NULL, proc, (void *)i );
	for( unsigned i = 0 ; i < num_hebras ; i++ )
	{
		unsigned long resultado ;
		pthread_join( id_hebra[i], (void **) (&resultado) );
		cout << "func(" << i << ") == " << resultado << endl;
	}
}




