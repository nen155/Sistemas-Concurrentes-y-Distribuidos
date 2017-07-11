
#include <iostream>
#include <iomanip>   /*setprecision*/
#include <stdlib.h>     /* atoi */
#include <pthread.h>
#include "fun_tiempo.h"



using namespace std;
unsigned long m = 0;  // número de muestras (un millón)
const unsigned n = 4; // número de hebras

double resultado_parcial[n]; // tabla de sumas parciales (una por hebra)

double f(double x)       // implementa función $f$
{
	return 4.0 / (1 + x*x);     // $~~~~f(x)\,=\,4/(1+x^2)$
}

double calcular_integral_secuencial()
{
	///MEDICION DEL TIEMPO EN SEGUNDOS
	struct timespec inicio = ahora() ;

	double suma = 0.0;                      // inicializar suma
	for (unsigned long i = 0; i < m; i++) // para cada $i$ entre $0$ y $m-1$
		suma += f((i + 0.5) / m);   // $~~~~~$ añadir $f(x_i)$ a la suma actual

	///MEDICION DEL TIEMPO EN SEGUNDOS
	struct timespec fin = ahora() ;
	cout << "tiempo transcurrido == " << duracion( &inicio, &fin ) << " seg." << endl ;
	return suma / m;                          // devolver valor promedio de $f$
}


void * funcion_hebra(void * ih_void) // función que ejecuta cada hebra
{
	unsigned long ih = (unsigned long)ih_void; // número o índice de esta hebra
	double sumap = 0.0;
	for (unsigned long i = 0; i < (m/n); i++) 
		sumap += f((i + 0.5) / (m/n));   
	resultado_parcial[ih] = sumap; // guardar suma parcial en vector.
	return NULL;
}

double calcular_integral_concurrente()
{	
	///MEDICION DEL TIEMPO EN SEGUNDOS
	struct timespec inicio = ahora() ;

	double resultado=0;
	// crear y lanzar $n$ hebras, cada una ejecuta "funcion\_concurrente"
	//.....
	pthread_t hebras[n];
	for( unsigned i = 0 ; i < n ; i++ )
		pthread_create( &(hebras[i]), NULL, funcion_hebra, (void *)i );

	// esperar (join) a que termine cada hebra, sumar su resultado
	//.....
	for( unsigned i = 0 ; i < n ; i++ )
	{
		pthread_join( hebras[i], NULL );
		resultado += resultado_parcial[i];
	}

	// devolver resultado completo
	// .....

	///MEDICION DEL TIEMPO EN SEGUNDOS
	struct timespec fin = ahora() ;
	cout << "tiempo transcurrido == " << duracion( &inicio, &fin ) << " seg." << endl ;

	return resultado/m; // (cambiar)
}

int main(int argc, char *argv[])
{
	m = atoi(argv[1]);
	cout << "Ejemplo 4 (cálculo de PI)" << endl;

	cout << "valor de PI (calculado secuencialmente) == " << setprecision(80) << calcular_integral_secuencial() << endl << "valor de PI (calculado concurrentemente) == " << setprecision(80) << calcular_integral_concurrente() << endl;

pthread_exit(NULL);

}
