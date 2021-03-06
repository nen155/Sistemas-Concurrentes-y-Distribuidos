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
	private boolean dormido;
	private Condition corta;
	private Condition clientEspera;
	private Condition clientPelando;
	public Barberia(){
		clientEspera = makeCondition();
		clientPelando = makeCondition();
		corta= makeCondition();
		pelando=false;
		dormido=true;
	}
	// invocado por los clientes para cortarse el pelo
	public void cortarPelo (int idCliente)
	{ 
		enter();
		if(dormido){
			System.out.println ("---- El cliente "+ idCliente+ " despierta al barbero"); 
			pelando=true;
			corta.signal();
			
			
		}
		if((!clientPelando.isEmpty() || !clientEspera.isEmpty())&&pelando) {
			System.out.println ("Cliente esperando--->"+idCliente); 
			clientEspera.await();
			System.out.println ("//////Me han desperando "+idCliente); 
		}
		
		
		System.out.println ("!!!!Me estoy pelando: "+idCliente);
			
		clientPelando.await();	
		System.out.println ("=======TERMINE DE PELARME====="+idCliente);
		leave();
	}
	// invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
	public void siguienteCliente ()
	{ 
		enter();
				
		if(!pelando && clientEspera.isEmpty()) {
			dormido=true;
			System.out.println("++++ Barbero esperando cliente");
			corta.await();
			
		}	
			dormido=false;
			pelando=true;
			clientEspera.signal();
		
		leave();
	}
	// invocado por el barbero para indicar que ha terminado de cortar el pelo
	public void finCliente ()
	{ 
		enter();
		pelando=false;
		System.out.println( "Barbero termina con cliente----");
		clientPelando.signal();		
		
		leave();
	}
}

class Cliente implements Runnable
{ 
	public Thread thr ;
	private Barberia barberia;
	int idCliente;
	public Cliente( int id,String nombre,Barberia barb)
	{ 
		idCliente=id;
		barberia = barb;
		thr = new Thread(this,nombre);
	}
	public void run ()
	{ 
		while (true) {
			barberia.cortarPelo (idCliente); // el cliente espera (si procede) y se corta el pelo
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
		    clie[i] = new Cliente(i,"Cliente "+i+":",barb) ;
		  // poner en marcha las hebras
		barbero.thr.start();
		for(int i = 0; i < 3; i++) 
			clie[i].thr.start();
		
	  }
}
