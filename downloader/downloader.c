#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>//ppt
#define URL_OBJETIVO "http://212.128.69.216/lolo" //SUSTITUIR POR ARCHIVO LOCAL PEQUEÑO
#define TAMANNO_OBJETIVO_REMOTO_BYTES 1047491658L //TAMAÑO ARCHIVO LOCAL
#define PREFIJO_NOMBREARCHIVO_FRAGMENTO "descarga"
int main(int argumento_numero, char* argumentos[]) {
    int tamanno_fragmento, proceso_numero, desde, hasta, pid,numero_procesos=atoi(argumentos[1]); //atoi es alguna funcion array a entero de una biblioteca
    int estado;//ppt?
    char modo_descarga=argumentos[2][0],archivo_salida[200]; //ppt
    valida_argumentos(argumento_numero, argumentos); //si no son validos dara error
    tamanno_fragmento = TAMANNO_OBJETIVO_REMOTO_BYTES/numero_procesos; // Calcula los tamaños de los fragmentos e informa al usuario
    printf ("Using %d processes for download \n", numero_procesos);
    if (TAMANNO_OBJETIVO_REMOTO_BYTES % numero_procesos == 0) {printf ("%d fragmentos de %d bytes\n",numero_procesos,tamanno_fragmento);}
    else {
        printf ("%d fragmentos de %d bytes\n", numero_procesos-1, tamanno_fragmento);
        printf ("%d fragmentos de %ld bytes\n", 1, TAMANNO_OBJETIVO_REMOTO_BYTES-(tamanno_fragmento*numero_procesos)+tamanno_fragmento);
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
        if (modo_descarga == 'S') {
            /**TODO: el padre debe esperar hasta que el hijo haya terminado de
             * descargar el fragmento actual si el modo de descarga es S
             * (sequencial)*/
        }
        if (modo_descarga == 'P') {
            for (proceso_numero=1; proceso_numero <= numero_procesos; proceso_numero++) { //codigo del hijo
                /**TODO: espera hasta que todas las descargas hayan terminado si el modo de descarga
                * es P (paralelo)*/
                sprintf(archivo_salida, "%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
                printf("%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
                printf("\t fragmento #%d: Rango %d-%d \n", proceso_numero, desde, hasta);
            }
        }
        sprintf(archivo_salida, "%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
        printf("%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso_numero);
        printf("\t fragmento #%d: Rango %d-%d \n", proceso_numero, desde, hasta);
    }
    printf ("-- Finaliza padre y downloader --\n");
    exit(0);
}
/**Ejemplo llamada curl :
 * curl -s -H "Rango: bytes=2-3" https://localhost/testfile.txt -o nombrearchivo */
//void descarga_fragmento(char* url, long desde, long hasta, char* archivo_salida);
void descarga_fragmento(char* url, long desde, long hasta, char* archivo_salida) {
    char rango[200];
    sprintf(rango, "Rango: bytes=%ld-%ld", desde, hasta);
    printf("Probando %s\n",rango);
    execl("curl", "curl", "-s", "-H", rango, url, "-o", archivo_salida, NULL);
    perror("Error"); //no deberia llegar hasta aqui ya que execlp reemplaza el codigo del proceso
}
//int valida_argumentos(int numero_argumento, char* argumentos[]);
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
    exit(error);
}
