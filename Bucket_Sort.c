#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main (int argc, char *argv[]){
    int float_size;
    float * arr;
    float current;
    FILE * fp;
    //double start,end,start2, end2;

    if (argc != 4){
        exit(1);
    }

    int num_threads = atoi(argv[1]);
    int num_bins = atoi(argv[2]);


    if(num_threads > 20 || num_threads < 1 || num_bins > 100 || num_bins < 1){
        exit(1);
    }

   
    fp = fopen(argv[3], "r");

    if(fp == NULL){
        printf("File does not exist\n");
        exit(1);
    }else{
        fscanf(fp, "%d", &float_size); 
        arr = (float *) malloc ( float_size * sizeof(float));

        int i = 0; 
        while( fscanf(fp, "%f", &current) != EOF){
            arr [i++] = current;
        }
        
    }
    
    /**
     for(int i = 0; i < float_size; i++){
         printf("Float is : %d  %f\n",i, arr[i]);
     }
**/

    fclose(fp);
   //dviide the floating point number by range of each bucket decide where they go


    //start = omp_get_wtime();
    int shared_bins [num_bins];
    float bin_range = 20.0/num_bins ;
    
        // each thread has its local array 
        // bin_ans, local bins are private
    omp_set_num_threads(num_threads);

    
    #pragma omp parallel
    {
        
        //start2 = omp_get_wtime();
        int local_bins [num_bins];
        
        #pragma omp for
        for(int i = 0; i < num_bins; i++){
            shared_bins [i] = 0;
        }

        for(int i = 0; i < num_bins; i++){
            #pragma omp for
            for(int j = 0; j < num_bins; j++){
                local_bins [i] = 0;
            }
        }
        


        #pragma omp for nowait
        for(int i = 0; i < float_size; i++){
                int bin_ans = (arr[i] / bin_range);
                local_bins[bin_ans] += 1;
        }

        
        for(int i = 0; i < num_bins; i++ ){
            #pragma omp atomic
            shared_bins [i] += local_bins[i];
        }
        //end2 = omp_get_wtime();
    
    }
    //end = omp_get_wtime();




    for(int i = 0; i < num_bins; i++){
        printf("bin[%d] = %d\n", i, shared_bins[i]);
    }
    
    //printf("Parallel Process took %f seconds \n", end2 - start2);
    //printf("Process took %f seconds \n", end - start);
}