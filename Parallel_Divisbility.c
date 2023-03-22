// must compile with -std=c99 -Wall -o checkdiv 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char *argv[]){
int comm_size;
int my_rank;

unsigned int x, n;
unsigned int i; //loop index
FILE * fp; //for creating the output file
char filename[100]=""; // the file name
int * arr;
//char * numbers; //the numbers in the range [2, N]

clock_t start_p1, start_p3, end_p1,  end_p3;
double start_p2, end_p2;


/////////////////////////////////////////
// start of part 1

start_p1 = clock();
// Check that the input from the user is correct.
if(argc != 3){

  printf("usage:  ./checkdiv N x\n");
  printf("N: the upper bound of the range [2,N]\n");
  printf("x: divisor\n");
  exit(1);
}  

n = (unsigned int)atoi(argv[1]); 
x = (unsigned int)atoi(argv[2]);



MPI_Init (&argc, &argv);


MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

// Process 0 must send the x and n to each process.
// Other processes must, after receiving the variables, calculate their own range.


if (my_rank == 0){
    for(int j = 1; j < comm_size; j++){
        MPI_Send(&n, 1, MPI_INT, j,0, MPI_COMM_WORLD);
        MPI_Send(&x, 1, MPI_INT, j,1 , MPI_COMM_WORLD);
        
    }
    
}else{
  for(int j = 1; j < comm_size; j++){
      if(my_rank == j)
      {
        MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&x, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }

  
}



end_p1 = clock();
//end of part 1
/////////////////////////////////////////


start_p2 = MPI_Wtime();
int load = (n - 2 + 1 ) / comm_size;
//incase of extra load
//    final_arr = (int *)malloc(comm_size * (load + comm_size) * sizeof(int));


arr = (int *)malloc ((load+comm_size) * sizeof(int));
//


for(i = 0; i < load + comm_size; i++){
    arr[i] = 0;
}
/////////////////////////////////////////
//start of part 2
// The main computation part starts here




//from 2 to 67 4 process    16 each
// 2 - 17, 18-33, 34-49, 50 - 65 

if ( (n - 2 +1 ) % comm_size == 0){// if equal load can be divided
    
    for (i = 0; i < comm_size; i++){ 
          int index = 0;

          if(my_rank == i){

            int range = 2+(i+1)*load;
            for(int j = i * load + 2; j < range; j++){
                if (j % x == 0){
                    arr[index] = j;
                    index++;
                }
                
            }
          }
     
    }
/**
    for(i = 0; i < load;i++){
        if(arr[i] != 0)
            printf("The num is:   %d\n", arr[i]);
    } 
**/
    

    
}else{
//
        for (i = 0; i < comm_size; i++){ 
          int index = 0;

          if(my_rank != comm_size-1 && my_rank == i){

            int range = 2+(i+1)*load;
            for(int j = i * load + 2; j < range; j++){
                if (j % x == 0){
                    arr[index] = j;
                    index++;
                }
                
            }
          }else if (my_rank == comm_size- 1 && my_rank == i){
              int range = 2+(i+1)*load;
              for(int j = i * load + 2; j < range + ((n-1) % comm_size ); j++){
                  if (j % x == 0){
                    arr[index] = j;
                    index++;
                }
              }
          }
     
    }



/**
    for(i = 0; i < load;i++){
        if(arr[i] != 0)
            printf("The num is:   %d\n", arr[i]);
     }
  **/
}




int * final_arr; //holds final result


//process 0 adds its own array to the final arr
if(my_rank == 0){
    final_arr = (int *)malloc(comm_size * (load + comm_size) * sizeof(int));

}


// process 0 receives all other arrays
MPI_Gather(arr,load + comm_size, MPI_INT, final_arr,load + comm_size, MPI_INT, 0, MPI_COMM_WORLD);

//printf("ok222\n");
end_p2 = MPI_Wtime();




/**
for(i = 0; i < comm_size; i++){
    if(my_rank == i){
    printf("I am process %d:   " , i);
    double part_2 = end_p2 - start_p2;
    printf("I am process %d ,  part 2 takes %lf \n",i,part_2);
   
    }

}
**/
double part_2_time = (end_p2 - start_p2);

double final_part2_time;
MPI_Reduce(&part_2_time, &final_part2_time, sizeof(double), MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD );


//printf("ok333\n");


if(my_rank == 0){
    
    

/**
    for(int j = 0; j < (comm_size * (load + comm_size)); j++){
        if(final_arr [j] != 0)
            printf("Final num is : %d\n" ,final_arr[j]);
    }
  **/
}








if (my_rank == 0){
start_p3 = clock();
    strcpy(filename, argv[1]);
    strcat(filename, ".txt");

    fp = fopen(filename, "w");

    if( !(fp = fopen(filename,"w+t")))
    {
    printf("Cannot create file %s\n", filename);
    exit(1);
    }



    for(int j = 0; j < (comm_size * (load + comm_size)); j++){
        fprintf(fp, "%d\n", final_arr[i]);  
    }
    fclose(fp);

    end_p3 = clock();

        
        printf("time of part1 = %lf s part2 = %lf s part3 = %lf s\n", 
            (double)(end_p1-start_p1)/CLOCKS_PER_SEC,
            (final_part2_time), 
            (double)(end_p3-start_p3)/CLOCKS_PER_SEC );
    
}







MPI_Finalize();
return 0;
}
