#include "utils.h"

//log tem de ter
//instante quando se abre o registo (o log)

//tem de ter uma cena  a contar o tempo no início do programa

int main(int argc, char *argv[], char *envp[]){

    char inst[MAX_FILE_NAME], pid[PID_NUMBER], act[MAX_FILE_NAME];
    struct timeb start, end;
    double diff;
    int i = 0;

    ftime(&start);

    while(i++ < 2)
        scanf("%lf", &diff);

    ftime(&end);

    diff = (double) (1000.00*(end.time - start.time) + (end.millitm - start.millitm));
    printf("%.2lf\n", diff); //tempo atual em milisegundos com 2 casa decimais, SPOILER(as casas decimais vao ser sempre 0)

    FILE *f;
    char logfilename[MAX_FILE_NAME + 1];

    for (int i = 0; envp[i] != NULL; i++)
        if (!strcmp("LOGFILENAME", envp[i]))
            strcpy(logfilename, envp[i]);

    strcat(logfilename,".txt");

    f = fopen(logfilename, "a+"); // a+ (create + append) option will allow appending which is useful in a log file

    if (f == NULL)
        return 1;

    return 0;
}