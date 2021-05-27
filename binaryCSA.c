/*
Coded by:   Emrullah Sonuç
            http://3mrullah.com

E-mail:     esonuc@karabuk.edu.tr
            esonuc@yandex.com

Paper:      Sonuç, E.
            Binary crow search algorithm for the uncapacitated facility location problem.
            Neural Computing & Applications (2021).

DOI:        https://doi.org/10.1007/s00521-021-06107-2

*/
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>
#include<float.h>

#define N 400          // Number of crows (Population Size)
#define AP 0.1         // Awareness probability
#define MAX_ITER 80000/N


double UFLP(int loc, int cus, double customer[][loc], double location[loc], bool *per)
{
    double cost = 0;
    double min_cost = 0;
    int i,j;

    for(i=0; i<loc; i++)
        if (per[i] == 1)
            cost = cost + location[i];

    for(i=0; i<cus; i++)
    {
        min_cost = DBL_MAX;
        for(j=0; j<loc; j++)
        {
            if (per[j] == 1 && customer[i][j]<min_cost)
                min_cost = customer[i][j];
        }
        cost = cost + min_cost;
    }

    return cost;
}

static inline double closed_interval_rand(double x0, double x1)
{
    return x0 + (x1 - x0) * rand() / ((double) RAND_MAX);
}

int main(int argc, char**argv)
{
    clock_t begin, end;
    double time_spent;
    begin = clock();
    srand(time(NULL));
    int cus, loc;
    int i,j,iter,control;

    char infile[80],indirectory[]="ORLIB-uncap\\\\",outfile[50]="results\\\\";
    if (argc != 2){
	  printf("Usage:\n\t%s input file\n", argv[0]);
	  exit(1);
    }
    strcpy(infile, argv[1]);
    strcat(indirectory,infile);
    FILE *fp = fopen(indirectory, "r");
    fscanf(fp, "%d", &loc);
    fscanf(fp, "%d", &cus);
    double location[loc];
    double customer[cus][loc];
    double temp;
    int tt;
    for(i=0;i<loc;i++)
    {
        fscanf(fp, "%s", &temp);
        fscanf(fp, "%lf", &location[i]);
    }

    for(i=0;i<cus;i++)
    {
        fscanf(fp, "%d",&tt);
        for(j=0;j<loc;j++)
        {
            fscanf(fp, "%lf", &customer[i][j]);
        }
    }

    double x[loc];
    int follow[N];

    double obj_crows[N] = {0};
    double obj_memory[N] = {0};

    bool x_crows[N][loc];
    bool x_memory[N][loc];


    memset( x_crows, 0, N*loc*sizeof(bool) );
    memset( x_memory, 0, N*loc*sizeof(bool) );

    double global_best = DBL_MAX;

    closed_interval_rand(0,1);


    // Memory initialization
    for(i=0; i<N; i++)
    {
        for(j=0; j<loc; j++)
        {
            if ( closed_interval_rand(0,1) < 0.5 )
            {
                x_crows[i][j] = 0;
                x_memory[i][j] = 0;
            }

            else
            {
                x_crows[i][j] = 1;
                x_memory[i][j] = 1;
            }

        }

    }

    for(i=0; i<N; i++)
    {
        obj_memory[i] = UFLP(loc, cus, customer, location, x_crows[i]);
    }

    // Iterations start
    for(iter=0; iter<MAX_ITER; iter++)
    {


        for(i=0; i<N; i++)
        {
            obj_crows[i] = UFLP(loc, cus, customer, location, x_crows[i]);

            if(obj_crows[i] < obj_memory[i])
            {
                obj_memory[i] = obj_crows[i];

                for(j=0; j<loc; j++)
                    x_memory[i][j] = x_crows[i][j];
            }

            if (obj_memory[i] < global_best)
                global_best = obj_memory[i];
        }

        for(i=0; i<N; i++)
            follow[i]=ceil(N*closed_interval_rand(0,1))-1;


        for(i=0; i<N; i++)
        {
            if( closed_interval_rand(0,1) > AP)
                for(j=0; j<loc; j++)
                {
                    x_crows[i][j] = ( x_memory[i][j] ^ ( ( rand() & 1) & ( (x_memory[follow[i]][j] ^ x_memory[i][j]) ) ) );
                }
            else
                for(j=0; j<loc; j++)
                {

                    if ( closed_interval_rand(0,1) < 0.5 )
                        x_crows[i][j] = 0;
                    else
                        x_crows[i][j] = 1;

                }
        }

        //printf("%.3f\n",global_best);

    }

    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;


    printf("\n\n Best: \t %.3f \n Time: \t %.3f sec.\n", global_best, time_spent);
    return 0;
}
