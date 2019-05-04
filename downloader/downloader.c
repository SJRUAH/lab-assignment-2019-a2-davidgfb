#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>//biblioteca para el wait
//#define URL_OBJETIVO "http://212.128.69.216/lolo" //SUSTITUIR POR ARCHIVO LOCAL PEQUEÑO
#define URL_OBJETIVO "archivo"
//#define TAMANNO_OBJETIVO_REMOTO_BYTES 1047491658L 
#define TAMANNO_OBJETIVO_REMOTO_BYTES 5868L //TAMAÑO ARCHIVO LOCAL d3fa600db6d81dcd94b531315c60a433
#define PREFIJO_NOMBREARCHIVO_FRAGMENTO "descarga"
int valida_argumentos(int numero_argumento, char* argumentos[]) {
    /**Primero,tenemos que comprobar el numero de argumentos, tambien, si el modo_descarga de
     *  argumentos es P (paralelo) o S (secuencial)*/
    char modo_descarga=argumentos[2][0];
    int numero_procesos=atoi(argumentos[1]);
    int error=-1;//si no cumple ninguna de las condiciones dara error | tiene que cumplir al menos 1 para que no de error
    if (numero_argumento != 3 ) {
        printf("error: numero invalido de argumentos\n uso: %s procesos {P/S} \n \t procesos: numero de procesos a descargar para fork \n \t modo descarga: (P) descarga paralela  (S) descarga secuencial \n", argumentos[0]);
        error=0;
    }
    if (modo_descarga != 'P' && modo_descarga != 'S') {
        printf("error: modo descarga invalido. Tiene que ser P o S \n");
        error=0;
    }
    if (numero_procesos <=0) {
        printf("error: el numero de procesos tiene que ser mayor que 0 \n");
        error=0;
    }
    return(error);
}
void descarga_fragmento(char* url, long desde, long hasta, char* archivo_salida) {
    /**Ejemplo llamada curl :
    * curl -s -H "Range: bytes=2-3" https://localhost/testfile.txt -o nombrearchivo */ //no vale rango
    char rango[200];
    sprintf(rango, "Range: bytes=%ld-%ld", desde, hasta);//no vale rango!
    printf("Probando %s\n",rango);
    execlp("curl", "curl", "-s", "-H", rango, url, "-o", archivo_salida, NULL);
    perror("Error"); //no deberia llegar hasta aqui ya que execlp reemplaza el codigo del proceso
}
int main(int argumento_numero, char* argumentos[]) {
    int tamanno_fragmento, proceso_numero, desde, hasta, pid,numero_procesos=atoi(argumentos[1]); //atoi es alguna funcion array a entero de una biblioteca
    int estado;//para el wait &estado
    char modo_descarga=argumentos[2][0],archivo_salida[200]; //nombre del archivo
    valida_argumentos(argumento_numero, argumentos); //si no son validos dara error
    tamanno_fragmento = TAMANNO_OBJETIVO_REMOTO_BYTES/numero_procesos; // Calcula los tamaños de los fragmentos e informa al usuario
    printf ("Usa %d procesos para la descarga \n", numero_procesos);
    if (TAMANNO_OBJETIVO_REMOTO_BYTES % numero_procesos == 0) {printf ("%d fragmentos de %d bytes\n",numero_procesos,tamanno_fragmento);}
    else {
        printf ("%d fragmentos de %d bytes\n", numero_procesos-1, tamanno_fragmento);
        printf ("%d fragmento de %ld bytes\n", 1, TAMANNO_OBJETIVO_REMOTO_BYTES-(tamanno_fragmento*numero_procesos)+tamanno_fragmento);
    }
    printf ("Total %ld bytes a descargar \n", TAMANNO_OBJETIVO_REMOTO_BYTES);
    
    for (proceso_numero=1; proceso_numero <= numero_procesos; proceso_numero++) {//Para cada fragmento, calcula el rango desde el que empieza y donde para
        if (numero_procesos == 1) {
            desde = 0;
            hasta = TAMANNO_OBJETIVO_REMOTO_BYTES;
        } else if (proceso_numero < numero_procesos) {
            desde = tamanno_fragmento*(proceso_numero-1);
            hasta = tamanno_fragmento*proceso_numero-1;
        } else {//El ultimo extiende el fragmento que queda hasta el final
            desde = hasta + 1;
            hasta = TAMANNO_OBJETIVO_REMOTO_BYTES;
        }
        /** TODO: Crea el proceso hijo que hara:
         *   - Escribe un mensaje mostrando el fragmento que descargara (sobre todo para propositos de depuracion
         *  (p.ej. printf("\t fragmento #%d: Rango %d-%d \n", proceso_numero, desde, hasta))
         *   - Genera el nombre del archivo para el fragmento actual, siguiendo el patron
         *  descarga-1, descarga-2, descarga-i. Puedes hacer esto con algo como:
         *       sprintf(archivo_salida, "%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
         *   - Llama a descarga_fragmento(URL_OBJETIVO, desde, hasta, archivo_); 
         *   - exit(0);*/
        
        pid = fork();
        if (pid == 0) { //Este codigo sera ejecutado por el proceso hijo
            printf("hijo #%d: Hola!\n",proceso_numero);
            printf("hijo #%d: Hecho! Hasta la vista\n",proceso_numero);
            descarga_fragmento(URL_OBJETIVO, desde, hasta, archivo_salida); 
            return(0); //El return aqui terminara la ejecucion del hijo
        }
        sprintf(archivo_salida, "%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
        printf("%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
        printf("\t fragmento #%d: Rango %d-%d \n", proceso_numero, desde, hasta);
        
        if (modo_descarga == 'S') {
            /**TODO: el padre debe esperar hasta que el hijo haya terminado de
             * descargar el fragmento actual si el modo de descarga es S
             * (sequencial)*/
            printf("padre: esperando a que el hijo #%d termine\n",proceso_numero);
            wait(&estado);
        }
        if (modo_descarga == 'P') {
            /*for (proceso_numero=1; proceso_numero <= numero_procesos; proceso_numero++) { //codigo del hijo
                TODO: espera hasta que todas las descargas hayan terminado si el modo de descarga
                * es P (paralelo)
                sprintf(archivo_salida, "%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
                printf("%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
                printf("\t fragmento #%d: Rango %d-%d \n", proceso_numero, desde, hasta);
            }*/
        }
        /*sprintf(archivo_salida, "%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
        printf("%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
        printf("\t fragmento #%d: Rango %d-%d \n", proceso_numero, desde, hasta);*/
    }
    printf ("-- Termina padre --\n");
    printf ("-- Termina downloader --\n");
    //exit(0);
}



