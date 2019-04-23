#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/wait.h>
#define TARGET_URL "http://212.128.69.216/lolo"
#define REMOTE_TARGET_SIZE_IN_BYTES 1047491658L
#define CHUNK_FILENAME_PREFIX "download"
void download_fragment(char* url, long from, long to, char* outfile);
int are_arguments_correct(int argc, char* argv[]);
int main(int argc, char* argv[]) {
    int chunk_size;
    int i;
    int from, to;
    int pid;
    int num_processes=atoi(argv[1]);
    int status;
    char download_mode=argv[2][0];
    char outfile[200];
    if (!are_arguments_correct(argc, argv)) {return -1;}
    chunk_size = REMOTE_TARGET_SIZE_IN_BYTES/num_processes;
    printf ("Using %d processes for download \n", num_processes);
    if (REMOTE_TARGET_SIZE_IN_BYTES % num_processes == 0) {printf ("%d chunks of %d bytes\n",num_processes,chunk_size);}
    else {
        printf ("%d chunks of %d bytes\n", num_processes-1, chunk_size);
        printf ("%d chunks of %ld bytes\n", 1, REMOTE_TARGET_SIZE_IN_BYTES-(chunk_size*num_processes)+chunk_size);
    }
    printf ("Total %ld bytes to download \n", REMOTE_TARGET_SIZE_IN_BYTES);
    for (i=1; i <= num_processes; i++) {
        if (num_processes == 1) {
            from = 0;
            to = REMOTE_TARGET_SIZE_IN_BYTES;
        } else if (i < num_processes) {
            from = chunk_size*(i-1);
            to = chunk_size*i-1;
        } else {
            from = to + 1;
            to = REMOTE_TARGET_SIZE_IN_BYTES;
        }
        if (download_mode == 'S') {}
        if (download_mode == 'P') {
            for (i=1; i <= num_processes; i++) { //codigo del hijo
                sprintf(outfile, "%s-%d", CHUNK_FILENAME_PREFIX, i);
                printf("%s-%d", CHUNK_FILENAME_PREFIX, i);
                printf("\t chunk #%d: Range %d-%d \n", i, from, to);
            }
        }
        sprintf(outfile, "%s-%d", CHUNK_FILENAME_PREFIX, i);
        printf("%s-%d", CHUNK_FILENAME_PREFIX, i);
        printf("\t chunk #%d: Range %d-%d \n", i, from, to);
        }
    printf ("-- End father and downloader --\n");
    exit(0);
}
void download_fragment(char* url, long from, long to, char* outfile) {
    char range[200];
    sprintf(range, "Range: bytes=%ld-%ld", from, to);
    printf("Testing %s\n",range);
    execlp("curl", "curl", "-s", "-H", range, url, "-o", outfile, NULL);
    perror("Error");
}
int are_arguments_correct(int argc, char* argv[]) {
    char download_mode=argv[2][0];
    int num_processes=atoi(argv[1]);
    int correcto=0;
    if (argc != 3 ) {printf("error: invalid number of arguments\n""usage: %s processes {P/S} \n""\tprocesses: number of download processes to fork \n""\tdownload mode: (P) Parallel download (S) Sequential download\n", argv[0]);}
    if (download_mode != 'P' && download_mode != 'S') {printf("error: invalid download mode. It has to be P or S\n");}
    if (num_processes <=0) {printf("error: the number of processes has to be greater than 0\n");}
    correcto=1;
    return correcto;
}
