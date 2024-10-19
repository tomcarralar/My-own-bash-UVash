//Tomas Carretero Alarcon 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
# define TAM 200
#include <time.h>

//Creo mi propia lista enlazada de un objeto que llamo nodo que contiene un vector de strings, que representan cada uno de los argumentos de un proceso, por lo tanto cada nodo corresponde a un proceso
typedef struct nodo{
	char *argumentos[TAM];
	struct nodo *sig;
}Proc;

//Creador de la lista
Proc *listaProc(Proc *Lista){
	Lista = NULL;
	return Lista;
}
// Annadir un proceso a la lista enlazada, cargando los argumentos del proceso correspondiente en su nodo.
Proc *addProc(Proc *Lista, char *argu[TAM]){

	Proc *nuevoProc, *aux;
	nuevoProc = (Proc*)malloc(sizeof(Proc));
	for (int i=0; i<TAM; i++){
		nuevoProc -> argumentos[i] = argu[i];
	}

	nuevoProc -> sig = NULL;

	if (Lista == NULL){ //El nuevo proceso sera el ultimo de la lista
		Lista = nuevoProc;
	}else{
		aux = Lista;
		while(aux->sig != NULL){
			aux = aux -> sig;

		}
		aux -> sig = nuevoProc;
	}
	return Lista;

}


int main (int argc, char *argv[]){
	char error_message[30] = "An error has occurred\n"; //mensaje de error estandar

	if (argc > 2){	//Si tenemos mas de dos argmumentos, se trata de una entrada incorrecta y termina
		fprintf(stderr, "%s", error_message);
		exit(1);
	}

	char* lectura = (char*)malloc(TAM); // Reservo espacio en memoria para la lectura de ordenes.
	char* lectura1 = lectura; // guardo una copia del puntuero anterior para liberar memoria correctamente

	FILE *ficheroIn; // puntero para fichero de entrada para el modo batch

	if (argc == 2){		// Si tenemos dos argumentos, el segundo sera el nombre del fichero, lo abrimos
		ficheroIn = fopen(argv[1], "r");
		
		if (ficheroIn == NULL){ //en caso de error con la apertura del fichero, liberamos memoria, mensaje de error y terminamos el programa debidamente
			free(lectura); 
			fprintf(stderr, "%s", error_message);
			exit(1);
		}

	}
	size_t tam = TAM; // cargo la constante para poder utilizar getline
	int salir = 0; //variable que solo podra tomar los valores 0 (seguir ejecutando ordenes) y 1 (terminar)
	char *ejecutar; //string para extraer un argumento de la lectura.
	int procesarLectura = 0; //variable que puede tomar 0 (la lectura implica ejectar) y  1 (la lectura por algun motivo, no debe ejecutarse).

	while ( salir == 0 ){	//bucle principal solo termina cuando la variable terinar cambie de 0
		procesarLectura = 0;  //antes de la lectura, suponemos que toda lecutra es correcta.

		if(argc == 2){	// si argc es 2 entonces, modo batch
			if(getline(&lectura, &tam, ficheroIn) ==-1){ //ante cualquier error en la lecutra el programa termina sin ejecutar la correspondiente lecutra.
				salir = 1;
				procesarLectura = 1;
			}
		}else{ // si argc es 1 (el resto de opciones ya se han tratado) modo interactivo.
			printf("UVash> ");
			getline(&lectura, &tam, stdin);
		}

		char *procesos[TAM]; //array de strings, cada string es una orden con sus argumentos (la redireccion tambien seran argmumentos) todo junto segun lo mando el usuario
		char *argumentosPi[TAM]; // array de strings, cada string sera momentaneamente una sola palabra, bien orden o bien argumento, de un proceso concreto.

		// nos aseguramos que ambos arrays previamente comentados se inicalicen vacios, para evitar errores.
		for(int j=0; j < TAM; j++)	procesos[j] = NULL;
		for(int j=0; j < TAM;  j++)	argumentosPi[j] = NULL;

		//si el primer simbolo de una lectura es ampersan, la lectura es erronea y no se debe ejecutar.
		if(strstr(lectura, "&") == lectura){
			fprintf(stderr, "%s", error_message);
			procesarLectura = 1;
		}
		//si el primer simbolo de la lectura es un salto de linea, no se debe ejectar la lectura pero esta no es erronea.
		if(strstr(lectura, "\n") == lectura)
			procesarLectura = 1;

		int numProcs = 0; // variable que contine el numero de procesos a ejecutar que contiene la lecutra.

		//Extraemos el string de cada proceso (orden) asi como el total de procesos.
		while( (ejecutar = strsep( &lectura, "&EOF\n")) != NULL && procesarLectura==0 ) {

			if (strlen(ejecutar) > 0){
				procesos[numProcs] = ejecutar;
				numProcs+=1;
			}
		}
		//Comprobamos que no sea una entrada de espacios, solo si tenemos un unico proceso y no se ha detectado que lectura no se deba ejecutar.
		if(numProcs == 1 && procesarLectura==0){
			char* lectura2 = (char*)malloc(tam);
			strcpy(lectura2, procesos[0]); // hacemso una copia de la lectura para no perturbarla
			procesarLectura = 1; // suponemos que es todo espacios, y si algun caracter no lo es, entonces lo cambiamos.
			while( (ejecutar = strsep(&lectura2, " ") ) != NULL){
				if(strlen(ejecutar)>0){
					procesarLectura = 0;
				}

			}
			free(lectura2); //liberamos la memoria de este if.
		}

		if (procesarLectura == 0){

			Proc *Lista = listaProc(Lista); // creamos la lista enlazada, inialmente sera NULL, por contruccion.

			for(int j = 0; j < numProcs; j++){ //Analizamos cada una de las ordenes (procesos) para extraer sus palabras

				int z = 0; //indice temporal para saber que palabra extraemos en el while siguiente.

				for(int w=0; w < TAM; w++)	argumentosPi[w] = NULL;//inicalizamos cada componente del array a NULL para evitar solapamientos
				// Extraemos cada una de las palabras de la orden (proceso) que no sean espacios o saltos de linea.
				while( (ejecutar = strsep( &procesos[j], " \t")) != NULL) {
					if (strlen(ejecutar) > 0){
						argumentosPi[z] = ejecutar;
						z+=1;
					}
				}
				if(argumentosPi[0] == NULL || !strcmp(argumentosPi[0],">")) { // si no hemos extraido ninguna palabra o bien la primera es el simbolo de redireccion, entonces este proceso (orden) es erronea y no ejecutamos nada de la lecutra.
					procesarLectura = 1;
					j = numProcs; // si es erronea paramos no hace falta seguir.

					fprintf(stderr, "%s", error_message);
				}
				Lista = addProc(Lista, argumentosPi);// annadimos los argumentos de la orden (proceso) a la lista de procesos.
			}

			Proc *Lista1 = Lista; // guardamos un puntero al primer nodo de la lista para luego liberar espacio.
			pid_t pid; //definimos los hijos.

			if(numProcs!=1){	// si el numero de procesos es 1 entonces no hay ejecucion paralela, otro caso se crean tantos hijos como indique numProcs.
				for(int j = 0; j < numProcs; j++){
					pid = fork();
					if(pid == 0)
						break;
					Lista = Lista->sig; // cada hijo debe tener un proceso distinto, por lo que hacemso que lista apunte al nodo contiguo
				}
			}
			//Solo entran los hijos en ejecucion paralela o bien sino es ejecucion paralela entrara el padre ya que el hijo aun no se habra creado, en el condicional anterior sino en uno posterior a los funciones que creadas por mi (proximos 2 if)
			if(pid==0 || (numProcs==1 && pid !=0)){

				if (!strcmp(Lista->argumentos[0], "exit")){ // primer comando built-in (exit)
					if (Lista -> argumentos[1] == NULL){
						salir = 1;
					}else
						fprintf(stderr, "%s", error_message);//si el comando trae argumentos, entonces error.
				}else{
					if(!strcmp(Lista->argumentos[0], "cd")){ // segundo comando built-in (exit)
						int cdBien = 1; 	// variable que puede tomar 1 (comando correcto) o 0 (comando incorrecto)

						for(int r = 2; r<TAM; r++){ // si alguno de los argumentos posteriores a la ruta es null entocnes, error
							if(Lista -> argumentos[r] != NULL){
								cdBien = 0;
							}
						}

						if(chdir(Lista->argumentos[1]) == -1 || cdBien == 0)
							fprintf(stderr, "%s", error_message);

						//Lista = Lista->sig;
					}else {

						// Comprobamos que no haya redireccion:
						int mayores = 0;
						int posicion = TAM;
						int r = 0;	//indice en el bucle while contiguo, representa el numero de argumento que se analiza
						while(Lista->argumentos[r]!=NULL){

							if(strstr(Lista -> argumentos[r],">") != NULL){
								mayores += 1;
								if(r < posicion) posicion = r;	//nos quedamos con el primer argumento que contenga la redireccion
							}
							r +=1;
						}
						//Fin de comprobacion de que no haya redireccion

						if(mayores > 1) { // solo puede haber un simbolo de redirecion, otro caso error
							fprintf(stderr, "%s", error_message);
						}else{
							if(numProcs==1) pid = fork(); //asinacion del hijo en el caso no paralelo.
							if(pid == 0 && mayores < 2){ // codigo solo de hijos

								if(mayores == 1){ //en caso de tener redireccion, obtenemos el fichero y lo habrimos

									if( strlen(Lista->argumentos[posicion]) > 1 ){ //distinguimos que el fichero este en el mismo argumento que el simbolo de redireccion o que no lo este (else).
										char *fichero = Lista->argumentos[posicion];
										strncpy(fichero, Lista->argumentos[posicion]+1, strlen(Lista->argumentos[posicion])); 	//nos quedamos solo con el nombre del fichero.

										int fd = open(fichero, O_WRONLY | O_CREAT | O_TRUNC);
										if(fd!=-1 && Lista->argumentos[posicion+1] == NULL){
										dup2(fd, 1 );
										dup2(fd, 2 );
										Lista -> argumentos[posicion] = NULL;
										}else{ 	//tratamos el caso de que no se habra correctamente o bien haya argumentos despues. ambos casos error.
											fprintf(stderr, "%s", error_message);
											Lista -> argumentos[0] = NULL;
										}
									}else{
										int fd = open(Lista->argumentos[posicion+1], O_WRONLY | O_CREAT | O_TRUNC);
										if(fd!=-1 && Lista->argumentos[posicion+2] == NULL){ 	//comprobamos que se haya abierto correctamente y que no haya mas argmumentos despues de la redireccion
										dup2(fd, 1 );
										dup2(fd, 2 );
										Lista -> argumentos[posicion] = NULL;

										}else{
											fprintf(stderr, "%s", error_message);
											Lista -> argumentos[0]= NULL;
										}
									}
								}
								// ejecutamos el comando y comprobamos que lo haga correctamente, sino es asi error.
								if (execvp( Lista -> argumentos[0] , Lista -> argumentos)==-1)
									fprintf(stderr, "%s", error_message);

								if(numProcs==1) exit(0); // Si no es ejecucio paralela hacemos el exit del hijo aqui

							}else{ 	// no hace falta poner condicional porque un padre en ejecucion paralela nunca llega hasta el if
								int status;
								wait(&status);
							}
						}
					}
				}

				if (numProcs!=1) exit(0);	//Exit de los hijos en la ejecucion paralela.
			}else if(numProcs!=1){ 	//solo en paralelo, ya que sino es paralelo, la espera del hijo se hace unas lineas arriba.
				for (int j=0; j<numProcs; j++){		// esperamos a todos los hijos
					int status;
					wait(&status);
				}
			}

			while(Lista1 != NULL){ 	//liberamos la memoria de la lista antes de la siguiente lectura.
				Lista = Lista1->sig;
				free(Lista1);
				Lista1 = Lista;
			}
	}

    }
	if(argc == 2) fclose(ficheroIn);
	free(lectura1); //liberamos la memoria de la lecutra.
	return 0; 	
}

