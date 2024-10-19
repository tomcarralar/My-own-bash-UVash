# My-own-shell-UVash

Se trata de la implementacion de un shell Unix simple llamado UVash. El shell es el corazón de la interfaz de línea de comandos y es central en el entorno de programación Unix/C. Dominar el uso del shell es necesario para convertirse en un programador competente en este mundo; saber cómo se construye el shell es el enfoque de este proyecto.

## Objetivos
Los objetivos específicos de esta tarea son:

- Familiarizarse con el entorno de programación Linux.
- Aprender cómo se crean, destruyen y gestionan los procesos.
- Exponerse a la funcionalidad necesaria en los shells.

## Descripción General
En esta tarea, implementarás un intérprete de línea de comandos (CLI) o, como se conoce más comúnmente, un shell. El shell debe operar de la siguiente manera básica: cuando escribes un comando (en respuesta a su prompt), el shell crea un proceso hijo que ejecuta el comando que ingresaste y luego solicita más entrada del usuario cuando haya terminado.

El shell que implementes será similar, pero más simple, que el que ejecutas todos los días en Unix. Si no sabes qué shell estás ejecutando, probablemente sea bash. Una cosa que debes hacer en tu tiempo libre es aprender más sobre tu shell, leyendo las páginas del manual o otros materiales en línea.

## Especificaciones del Programa
### Shell Básico: UVash
Tu shell básico, llamado UVash (abreviatura de UVa Shell, naturalmente), es básicamente un bucle interactivo: imprime repetidamente un prompt UVash> (nota el espacio después del signo de mayor que), analiza la entrada, ejecuta el comando especificado en esa línea de entrada y espera a que el comando termine. Esto se repite hasta que el usuario escriba exit. El nombre de tu ejecutable final debe ser UVash.

El shell puede invocarse sin argumentos o con un solo argumento; cualquier otra cosa es un error. Aquí está la forma sin argumentos:

```
prompt> ./UVash
UVash>
```

En este punto, UVash está ejecutándose y listo para aceptar comandos. ¡Escribe!

Este modo se llama modo interactivo y permite al usuario escribir y ejecutar comandos uno a la vez. UVash también puede ejecutar en modo batch, donde lee comandos de un archivo y los ejecuta:

```
prompt> ./UVash batch.txt
```

### Ejecución de Comandos
Para ejecutar un comando, UVash debe hacer lo siguiente:

1. Crear un nuevo proceso con fork().
2. En el proceso hijo, usar execvp() para ejecutar el comando.
3. El proceso padre debe esperar a que el proceso hijo termine con wait() o waitpid().

### Manejo de Comandos Internos
UVash debe manejar ciertos comandos internos como exit, que termina el shell, y cd, que cambia el directorio de trabajo actual

Redirección de Entrada y Salida
El shell debe soportar la redirección de entrada (<) y salida (>). Por ejemplo:

```
UVash> ls > out.txt
UVash> sort < out.txt
```
### Ejecución en Paralelo
UVash debe soportar la ejecución de comandos en paralelo usando el operador &. Por ejemplo:
```
UVash> ls & sort
```

## Instalación
Para compilar el shell, usa el siguiente comando:
```
gcc -o UVash UVash.c -Wall -Werror
```
## Uso
### Modo Interactivo
Para iniciar el shell en modo interactivo:
```
./UVash
```
### Modo Batch
Para ejecutar comandos desde un archivo en modo batch:
```
./UVash batch.txt
```
