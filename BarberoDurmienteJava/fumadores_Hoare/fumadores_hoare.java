import monitor.* ;
import java.util.Random ;

class Estanco extends AbstractMonitor
{  
	private int ingre;
	private Condition ponIngrediente;
	private Condition[] fuma;
	public Estanco(){
		fuma = new Condition[3];
		fuma[0] =  makeCondition();
		fuma[1] = makeCondition();
		fuma[2] = makeCondition();
		ponIngrediente= makeCondition();
		ingre=-1;
	}
	
	// invocado por cada fumador, indicando su ingrediente o numero
	public void obtenerIngrediente( int miIngrediente )
	{ 
		enter();
		
		if(ingre != miIngrediente){ fuma[miIngrediente].await(); }
		ingre=-1;
		//Pongo los mensajes dentro de la exclusion para que se sincronice
		System.out.println( "Fumador "+miIngrediente+": fumando");
		ponIngrediente.signal(); 
		leave();
	}
	// invocado por el estanquero, indicando el ingrediente que pone
	public void ponerIngrediente( int ingrediente )
	{ 
		enter();
		ingre = ingrediente;
		//Pongo los mensajes dentro de la exclusion para que se sincronice
		System.out.println( "Estanquero pone: "+ingrediente);
		fuma[ingrediente].signal();
		leave();
	}
	// invocado por el estanquero
	public void esperarRecogidaIngrediente()
	{ 
		enter();
		System.out.println( "Estanquero a la espera----");
		if(ingre!=-1){ ponIngrediente.await(); } 
		leave();
	}

}
class Fumador implements Runnable
{
	int miIngrediente;
	private Estanco estanco;
	public Thread thr ;

	public Fumador( int p_miIngrediente, String nombre,Estanco est)
	{ 
		estanco = est;
		miIngrediente = p_miIngrediente;
		thr = new Thread(this,nombre);
	}
	public void run()
	{
		while ( true )
		{ 
			estanco.obtenerIngrediente( miIngrediente );
			aux.dormir_max( 2000 );
		}
	}
}
class Estanquero implements Runnable
{ 
	public Thread thr ;
	private Estanco estanco;
	public Estanquero(String nombre,Estanco est)
	{ 
		estanco = est;
		thr = new Thread(this,nombre);
	}
	public void run()
	{
	 int ingrediente ;
		while (true)
		{
			ingrediente = (int) (Math.random () * 3.0); // 0,1 o 2
			estanco.ponerIngrediente( ingrediente );
			estanco.esperarRecogidaIngrediente() ;
		}
	}
}


// calcular un numero aleatorio de milisegundos (entre 1/10 y 2 segundos)
class aux
{
  static Random genAlea = new Random() ;
  static void dormir_max( int milisecsMax )
  { try
    { Thread.sleep( genAlea.nextInt( milisecsMax ) ) ;
    } 
    catch( InterruptedException e )
    { System.err.println("sleep interumpido en 'aux.dormir_max()'");
    }
  }
}
class ProbarFumadores{
//Hay que representar el estado en el problema, el problema tiene 4
//estados posibles
//estado inicial que no hay nada, que hay papel, que hay cerillas o tabaco
//Si no esta su ingrediente en la mesa se bloquea.
	public static void main( String[] args ) 
	{ 
		Estanco est= new Estanco();
		Estanquero estanq = new Estanquero("Estanquero: ",est);
		Fumador[]  fum = new Fumador[3] ;
		for(int i = 0; i < 3; i++)
		    fum[i] = new Fumador(i,"Fumador"+i+":",est) ;
		  // poner en marcha las hebras
		estanq.thr.start();
		for(int i = 0; i < 3; i++) 
			fum[i].thr.start();
		
	  }
}
