#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

char * file_name;
char *files[3];
int a=0;
int height;
int width;
int height2;
int width2;
struct Matrix
{
    int ** Matrix_A;
    int ** MAtrix_B;
    int ** MAtrix_C;
    int start;
    int end;
    FILE *fp;
};
/// this function is used to read boundries of matrix from file
void read_boundries_from_file(int index[],int size)
{
    file_name=files[a];
    char c[1000];
    char *str;
    char *str2;
    char *str3;
    FILE *fptr;
    if ((fptr = fopen(file_name, "r")) == NULL) {
        printf("Error! File cannot be opened.");
        exit(1);
    }
    fscanf(fptr, "%[^\n]", c);
    str3=c;
    str3=strtok(str3," ");
    str=str3;
    while (str3!=NULL)
    {
        str3=strtok(NULL," ");
        if(str3!=NULL)
        {
            str2=str3;
        }
    }
    str=strtok(str," =");
    while (str!=NULL)
    {
        str=strtok(NULL,"=");
        if(str!=NULL)
        {
            sscanf(str, "%d", &index[0]);
        }
    }
    str2=strtok(str2," =");
    while (str2!=NULL)
    {
        str2=strtok(NULL,"=");
        if(str2!=NULL)
        {
            sscanf(str2, "%d", &index[1]);
        }
    }
    fclose(fptr);
    a++;
}
/// this function is used to read matrix number from file
int** read_matrix_from_file(int height,int width,int **matrix_A)
{
    FILE *fp;
    int i;
    int j;
    fp = fopen(file_name, "r"); // read mode
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    fscanf(fp, "%*[^\n]");
    for(i = 0; i < height; i++)
    {
        for(j = 0; j < width; j++)
        {
            if (!fscanf(fp, "%d", &matrix_A[i][j]))
                break;
        }
    }
    fclose(fp);
    return matrix_A;
}
/// this function is used to multiply matrix using only one thread
void* Matrix_MULTIPLY( void* W) {
    int i;
    int j, k;
    struct Matrix *B;
    B=(struct Matrix *)W;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width2; j++) {
            B->MAtrix_C[i][j]=0;
            for ( k = 0; k < height2; k++)
            {
                (B->MAtrix_C[i][j]) += (B->Matrix_A[i][k]) * (B->MAtrix_B[k][j]);
            }
        }
    }
}
/// this function is used to multiply matrix using a thread for each row
void* Matrix_MULTIPLY2( void* C) {
    int j, k;
    struct Matrix *B;
    B=(struct Matrix *)C;
    for (j = 0; j < width2; j++) {
        B->MAtrix_C[B->start][j]=0;
        for ( k = 0; k < height2; k++)
        {
            B->MAtrix_C[B->start][j] += B->Matrix_A[B->start][k] * B->MAtrix_B[k][j];
        }
    }
    free(C);
}
/// this function is used to multiply matrix using a thread for each element
void* Matrix_MULTIPLY3( void* C) {
    int  k;
    struct Matrix *B;
    B=(struct Matrix *)C;
    B->MAtrix_C[B->start][B->end]=0;
    for ( k = 0; k < height2; k++)
    {
        B->MAtrix_C[B->start][B->end] += B->Matrix_A[B->start][k] * B->MAtrix_B[k][B->end];
    }
    free(C);
}
int main(int argc, char *argv[] ) {
    char temp[100];
    char temp2[100];
    char temp3[100];
    if(argc < 2){
        files[0]="a.txt";
        files[1]="b.txt";
        files[2]="c_per_matrix.txt";
        files[3]="c_per_row.txt.txt";
        files[4]="c_per_element.txt";
    }
    else if(argc == 2){
        files[0]=argv[1];
        files[1]="b.txt";
        files[2]="c_per_matrix.txt";
        files[3]="c_per_row.txt.txt";
        files[4]="c_per_element.txt";
    }
    else if(argc == 3){
        files[0]=argv[1];
        files[1]=argv[2];
        files[2]="c_per_matrix.txt";
        files[3]="c_per_row.txt.txt";
        files[4]="c_per_element.txt";
    }
    else{
        files[0]=argv[1];
        files[1]=argv[2];
        strcpy(temp,argv[3]);
        strcpy(temp2,argv[3]);
        strcpy(temp3,argv[3]);
        strcat(temp,"_per_matrix.txt");
        strcat(temp2,"_per_row.txt");
        strcat(temp3,"_per_element.txt");
        files[2]=temp;
        files[3]=temp2;
        files[4]=temp3;
    }
    struct timeval stop, start;
    int index[2];
    read_boundries_from_file(index,2);
    height=index[0];
    width=index[1];
    int **matrix_A = (int **)malloc(index[0]*sizeof(int*));
    for (int i = 0; i < index[0]; ++i) {
        matrix_A[i]=(int *) malloc(index[1]*sizeof (int));
    }
    matrix_A= read_matrix_from_file(index[0],index[1],matrix_A);
    read_boundries_from_file(index,2);
    height2=index[0];
    width2=index[1];
    int **matrix_B = malloc(index[0]*sizeof(int*));
    for (int i = 0; i < index[0]; ++i) {
        matrix_B[i]= malloc(index[1]*sizeof (int));
    }
    matrix_B= read_matrix_from_file(index[0],index[1],matrix_B);
    int **matrix_C = malloc(height*sizeof(int*));
    for (int i = 0; i < height; ++i) {
        matrix_C[i]= malloc(width2*sizeof (int));
    }
    if(width!=height2)
    {
        printf("Matrix maultplication cannot be done\n ");
        return 0;
    }
    struct Matrix *A;
    A=malloc(sizeof(struct Matrix));
    A->Matrix_A=matrix_A;
    A->MAtrix_B=matrix_B;
    A->MAtrix_C=matrix_C;
    A->start=0;
    pthread_t t;
    gettimeofday(&start, NULL);
    pthread_create(&t,NULL,Matrix_MULTIPLY,(void *)A);
    pthread_join(t,NULL);
    gettimeofday(&stop, NULL);
    printf("Method: A thread per matrix\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("%s %d","Number of threads used :",1);
    printf("\n");
    printf("\n");
    A->fp = fopen(files[2], "w");
    fprintf(A->fp, "Method: A thread per matrix");
    fprintf(A->fp, "\n");
    fprintf(A->fp, "%s%d  %s%d", "row=",height,"column=",width2);
    fprintf(A->fp, "\n");
    fprintf(A->fp, "\n");
    for(int i=0 ;i<height;i++)
    {
        for (int j=0;j<width2;j++)
        {
            fprintf(A->fp, "%d  ", matrix_C[i][j]);
            fprintf(A->fp, "\t");
        }
        fprintf(A->fp, "\n");
    }
    pthread_t th[height];
    int id;
    int id3;
    int count=0;
    gettimeofday(&start, NULL);
    for(id=0;id<height;id++)
    {
        count++;
        struct Matrix *A;
        A=malloc(sizeof(struct Matrix));
        A->Matrix_A=matrix_A;
        A->MAtrix_B=matrix_B;
        A->MAtrix_C=matrix_C;
        A->start=id;
        pthread_create(&th[id],NULL,Matrix_MULTIPLY2,(void *) A);
    }
    for(id3=0;id3<height;id3++)
    {
        if(pthread_join(th[id3],NULL)!=0)
        {
            perror("Failed to join thread");
        }
    }
    gettimeofday(&stop, NULL);
    printf("Method: A thread per row\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("%s %d","Number of threads used :",count);
    printf("\n");
    A->fp = fopen(files[3], "w");
    fprintf(A->fp, "Method: A thread per row");
    fprintf(A->fp, "\n");
    fprintf(A->fp, "%s%d  %s%d", "row=",height,"column=",width2);
    fprintf(A->fp, "\n");
    fprintf(A->fp, "\n");
    for(int i=0 ;i<height;i++)
    {
        for (int j=0;j<width2;j++)
        {
            fprintf(A->fp, "%d  ", matrix_C[i][j]);
            fprintf(A->fp, "\t");
        }
        fprintf(A->fp, "\n");
    }
    printf("\n");
    pthread_t th2[height][width2];
    int id4;
    count=0;
    gettimeofday(&start, NULL);
    for(id=0;id<height;id++)
    {
        for(id4=0;id4<width2;id4++)
        {
            count++;
            struct Matrix *A;
            A=malloc(sizeof(struct Matrix));
            A->Matrix_A=matrix_A;
            A->MAtrix_B=matrix_B;
            A->MAtrix_C=matrix_C;
            A->start=id;
            A->end=id4;
            pthread_create(&th2[id][id4],NULL,Matrix_MULTIPLY3,(void *) A);
        }
    }
    for(id=0;id<height;id++)
    {
        for(id4=0;id4<width2;id4++)
        {
            if(pthread_join(th2[id][id4],NULL)!=0)
            {
                perror("Failed to join thread");
            }
        }
    }
    gettimeofday(&stop, NULL);
    printf("Method: A thread per element\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("%s %d","Number of threads used :",count);
    A->fp = fopen(files[4], "w");
    fprintf(A->fp, "Method: A thread per element");
    fprintf(A->fp, "\n");
    fprintf(A->fp, "%s%d  %s%d", "row=",height,"column=",width2);
    fprintf(A->fp, "\n");
    fprintf(A->fp, "\n");
    for(int i=0 ;i<height;i++)
    {
        for (int j=0;j<width2;j++)
        {
            fprintf(A->fp, "%d  ", matrix_C[i][j]);
            fprintf(A->fp, "\t");
        }
        fprintf(A->fp, "\n");
    }
    printf("\n");
    for (int i = 0; i < height; ++i) {
         free(matrix_A[i]);
    }
    free(matrix_A);
    for (int i = 0; i < height2; ++i) {
        free(matrix_B[i]);
    }
    free(matrix_B);
    for (int i = 0; i < height; ++i) {
        free(matrix_C[i]);
    }
    free(matrix_C);
    free(A);
    return 0;
}
