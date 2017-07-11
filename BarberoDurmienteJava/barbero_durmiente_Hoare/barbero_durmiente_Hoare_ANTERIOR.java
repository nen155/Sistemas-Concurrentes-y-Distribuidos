import monitor.* ;
import java.util.Random ;

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

class Barberia extends AbstractMonitor
{ 
	private boolean pelando;
	//private boolean dormido;
	private Condition corta;
	private Condition clients;
	private int i;

	public Barberia(){
		clients = makeCondition();
		corta= makeCondition();
		pelando=false;
		//dormido=true;
	}
	// invocado por los clientes para cortarse el pelo
	public void cortarPelo ()
	{ 
		enter();
		
		if(pelando) {
			//System.out.println( "Cliente esperando ");
			clients.await();
		}
		
		//if(dormido){
		if(!pelando){
			
		//	dormido=false;
			corta.signal();	
			System.out.println ("---- El cliente despierta al barbero"); 
		//}
		}
		leave();
	}
	// invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
	public void siguienteCliente ()
	{ 
		enter();
		if(!pelando && clients.isEmpty()) {
			System.out.println("++++ Barbero esperando cliente");
			//dormido=true;
			//pelando=false;
			corta.await();
		}
		pelando=true;
		System.out.println( "Cliente cortandose el pelo---- "+(i));
		leave();
	}
	// invocado por el barbero para indicar que ha terminado de cortar el pelo
	public void finCliente ()
	{ 
		enter();
		pelando=false;
		System.out.println( "Barbero termina con cliente----"+(i++));
		
		//dormido=true;
		clients.signal();		
		
		leave();
	}
}

class Cliente implements Runnable
{ 
	public Thread thr ;
	private Barberia barberia;
	public Cliente( String nombre,Barberia barb)
	{ 
		barberia = barb;
		thr = new Thread(this,nombre);
	}
	public void run ()
	{ 
		while (true) {
			barberia.cortarPelo (); // el cliente espera (si procede) y se corta el pelo
			aux.dormir_max( 2000 ); // el cliente esta fuera de la barberia un tiempo
		}
	}
}
class Barbero implements Runnable
{ 
	public Thread thr ;
	private Barberia barberia;
	public Barbero(String nombre,Barberia barb)
	{ 
		barberia = barb;
		thr = new Thread(this,nombre);
	}
	public void run ()
	{
		while (true) {
			barberia.siguienteCliente ();
			aux.dormir_max( 2500 ); // el barbero esta cortando el pelo
			barberia.finCliente ();
		}
	}
}

class ProbarBarbero{
//Hay que representar el estado en el problema, el problema tiene 4
//estados posibles
//estado inicial que no hay nada, que hay papel, que hay cerillas o tabaco
//Si no esta su ingrediente en la mesa se bloquea.
	public static void main( String[] args ) 
	{ 
		Barberia barb= new Barberia();
		Barbero barbero = new Barbero("Barbero: ",barb);
		Cliente[]  clie = new Cliente[3] ;
		for(int i = 0; i < 3; i++)
		    clie[i] = new Cliente("Cliente "+i+":",barb) ;
		  // poner en marcha las hebras
		barbero.thr.start();
		for(int i = 0; i < 3; i++) 
			clie[i].thr.start();
		
	  }
}
