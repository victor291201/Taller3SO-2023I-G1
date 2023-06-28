#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/shm.h>


pthread_barrier_t mybarrier;
pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;
void* funcion_hilo(void*);
struct data{
    int start;
    int end;
    int id;
};
int *vector;
int *resultado;
int ** resultadosV;
int nVect;
int nH;
int max_lenght = 20;
int change=0;
int times =0;
int nTimes =0;

int main(int args, char ** argsv){
    char *filen = argsv[1];
    FILE *file;
    file = fopen(filen,"rt");
    char *line = (char *) malloc( max_lenght*sizeof(char));
    fgets(line, max_lenght*sizeof(char), file);
    nTimes =atoi(line);
    nH = atoi(argsv[2]);
    printf("tendre [%d] hilos \n",nH);
    pthread_barrier_init(&mybarrier, NULL, nH+1);
    pthread_t ids[nH];
    struct data val[nH];
    int salto = 10000/nH; 
    for(int i = 0;i<nH;i++){
        val[i].start = salto*i;
        val[i].end = salto*(i+1);
        val[i].id = i;
        pthread_create(&ids[i],NULL,funcion_hilo,&val[i]);
    }
    while(times<nTimes){
        fgets(line, max_lenght*sizeof(char), file);
        nVect = atoi(line);
        vector = (int *) malloc(nVect*sizeof(int));
        for(int i = 0;i<nVect;i++){
            fgets(line, max_lenght*sizeof(char), file);
            vector[i]= atoi(line);
        }
        resultadosV = (int **) malloc(nH*sizeof(int*));
        for(int i = 0;i<nH;i++){
            int *vectvar = (int *) malloc(nVect*sizeof(int));
            for(int j = 0; j<nVect;j++){
                vectvar[j]=-1;
            }
            resultadosV[i] = vectvar;
        }
        resultado = (int *) malloc(nVect*sizeof(int));
        for(int i = 0;i<nVect;i++){
            resultado[i]= -1;
        }
        printf("el vector de entrada es: \n");
        for(int i = 0;i<nVect;i++){
            printf("%d ",vector[i]);
        }
        printf("\n");
        pthread_barrier_wait(&mybarrier);
        pthread_barrier_wait(&mybarrier);
        int cont = 0;
        for(int i = 0;i<nH;i++){
            for(int j = 0;j<nVect;j++){
                if(resultadosV[i][j]!=-1){
                resultado[cont]= resultadosV[i][j];
                cont ++;
                }
            }
        }
        printf("el vector de salida es: \n");
        for(int i = 0;i<nVect;i++){
            printf("%d ",resultado[i]);
        }
        printf("\n");
        times++;
    }
    for(int i = 0;i<nH;i++){
        pthread_join(ids[i],NULL);
    }
    pthread_barrier_destroy(&mybarrier);
    
    return 0;
}
void* funcion_hilo(void* arg){
    struct data* data = (struct data*)arg;
    printf("al hilo [%lu] le toco el rango[%d]-[%d]\n",pthread_self(),data->start,data->end);
    while(times<nTimes){
        pthread_barrier_wait(&mybarrier);
        int inicio=0;
        int final = 0;
        int var[nVect];
        for(int i = 0;i<nVect;i++){
            var[i]=-1;
        }
        for(int i = 0;i<nVect;i++){
            if(vector[i]>=data->start && vector[i]<data->end){
                var[final]=vector[i];
                final++;
            }
        }
        for(int i = 0;i<nVect;i++){
            int temp =0;
            for(int j=0;j<nVect-1;j++){
                if(var[j]>var[j+1]){
                    temp = var[j];
                    var[j] = var[j+1];
                    var[j+1]=temp;
                }
            }
        }
        for(int i = 0;i<nVect;i++){
            resultadosV[data->id][i]=var[i];
        }
        pthread_barrier_wait(&mybarrier);                                                                                                                                                    sleep(2);
    }
    
    
    pthread_exit(0);
}