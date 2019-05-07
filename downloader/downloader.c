#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>//biblioteca para el wait
#define URL_OBJETIVO "http://212.128.69.216/lolo2.txt"
#define TAMANNO_OBJETIVO_REMOTO_BYTES 333L 
#define PREFIJO_NOMBREARCHIVO_FRAGMENTO "descarga"
int valida_argumentos(int numero_argumento, char* argumentos[]) {
    /**Primero,tenemos que comprobar el numero de argumentos, tambien, si el modo_descarga de
     *  argumentos es P (paralelo) o S (secuencial)*/
    char modo_descarga=argumentos[2][0];
    int numero_procesos=atoi(argumentos[1]);
    int error=-1;//si no cumple ninguna de las condiciones dara error | tiene que cumplir al menos 1 para que no de error
    if (numero_argumento != 3 ) { //numero argumentos
        printf("error: numero invalido de argumentos\nuso: %s procesos {P/S}\n\tprocesos: numero de procesos a descargar para fork\n\tmodo descarga: (P) descarga paralela (S) descarga secuencial\n", argumentos[0]);
        error=0;
    }
    if (modo_descarga != 'P' && modo_descarga != 'S') {
        printf("error: modo descarga invalido. Tiene que ser P o S\n");
        error=0;
    }
    if (numero_procesos <=0) {
        printf("error: el numero de procesos tiene que ser mayor que 0\n");
        error=0;
    }
    return(error);
}
void descargaFragmento(char* url, long desde, long hasta, char* archivo_salida) {
    char rango[200];
    sprintf(rango, "Range: bytes=%ld-%ld", desde, hasta); //no vale rango!
    //printf("Probando %s\n",rango);
    execlp("curl", "curl", "-s", "-H", rango, url, "-o", archivo_salida, NULL); //ignora clp
    perror("Error"); //no deberia llegar hasta aqui ya que execlp reemplaza el codigo del proceso
}
int main(int procesos, char* descarga[]) {
    int tamanno_fragmento, desde, hasta, pid,numeroProcesos=atoi(descarga[1]); 
    char archivo_salida[200],modoDescarga=descarga[2][0]; //nombre del archivo
    valida_argumentos(procesos, descarga); //si no son validos dara error
    tamanno_fragmento = TAMANNO_OBJETIVO_REMOTO_BYTES/numeroProcesos; // Calcula los tamaños de los fragmentos e informa al usuario
    printf ("Usa %d procesos para la descarga \n", numeroProcesos);
    if (TAMANNO_OBJETIVO_REMOTO_BYTES % numeroProcesos == 0) {printf ("%d fragmentos de %d bytes\n",numeroProcesos,tamanno_fragmento);}
    else {
        printf ("%d fragmentos de %d bytes\n", numeroProcesos-1, tamanno_fragmento);
        printf ("%d fragmento de %ld bytes\n", 1, TAMANNO_OBJETIVO_REMOTO_BYTES-(tamanno_fragmento*numeroProcesos)+tamanno_fragmento);
    }
    printf ("Total %ld bytes a descargar \n", TAMANNO_OBJETIVO_REMOTO_BYTES);
    for (int proceso=1; proceso <= numeroProcesos; proceso++) {
        pid = fork();
        if (numeroProcesos == 1) {
            desde = 0;
            hasta = TAMANNO_OBJETIVO_REMOTO_BYTES;
        } else if (proceso < numeroProcesos) {
            desde = tamanno_fragmento*(proceso-1);
            hasta = tamanno_fragmento*proceso-1;
        } else { //El ultimo extiende el fragmento que queda hasta el final
            desde = hasta + 1;
            hasta = TAMANNO_OBJETIVO_REMOTO_BYTES;
        }
        sprintf(archivo_salida, "%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso);
        printf("%s-%d", PREFIJO_NOMBREARCHIVO_FRAGMENTO, proceso);
        printf("\t fragmento #%d: Rango %d-%d \n", proceso, desde, hasta);
        if (pid == 0) { //Este codigo sera ejecutado por el proceso hijo
            printf("hijo #%d: Hola!\n",proceso);
            descargaFragmento(URL_OBJETIVO,desde,hasta,archivo_salida);
            printf("hijo #%d: Hecho! Adios\n",proceso);
            return(0); //El Return aqui acabara la ejecucion del hijo
        }
        if (modoDescarga == 'S') { //Solo el padre llegara a esta linea
            printf("padre: esperando a que el hijo #%d finalice\n",proceso);
            wait(NULL);
        }
    }
    if (modoDescarga == 'P') {
        for (int proceso=1; proceso <= numeroProcesos; proceso++) {
            printf("padre: esperando a que el hijo #%d finalice\n",proceso);
            wait(NULL);
        }
    }
    printf ("-- Termina padre --\n");
    exit (0);
}

