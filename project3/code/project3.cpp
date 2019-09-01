#include <iostream>
#include <cstdlib>
#include <pthread.h>
 
using namespace std;
 
#define M   3
#define K   2
#define N   3
#define NUM_THREADS     M * N

int A[M][K] = { {1,4}, {2,5}, {3,6} };
int B[K][N] = { {8,7,6}, {5,4,3} };
int C[M][N];

struct thread_data{
   int row, col;
};
 
void *CalMatrix(void *threadarg)
{
   struct thread_data *my_data;
 
   my_data = (struct thread_data *) threadarg;
 
   int res = 0;
   for (int k = 0; k < K; k++){
      res += A[my_data->row][k]*B[k][my_data->col];
   }
   C[my_data->row][my_data->col] = res;
 
   pthread_exit(NULL);
}
 
int main ()
{
   pthread_t threads[NUM_THREADS];
   struct thread_data td[NUM_THREADS];
   int rc;
   int i, j;
   void *status;
   
   cout<<"Matrix A:"<<endl;
   for( i = 0; i< M; i++){
      for( j = 0; j < K; j++){
         cout<<A[i][j]<<' ';
      }
      cout<<endl;
   }

   cout<<"Matrix B:"<<endl;
   for( i = 0; i< K; i++){
      for( j = 0; j < N; j++){
         cout<<B[i][j]<<' ';
      }
      cout<<endl;
   }

   for( i = 0; i< M; i++){
      for( j = 0; j < N; j++){
         int n = i * M + j;
         cout <<"Creating thread, " << n + 1<< endl;
         td[n].row = i;
         td[n].col = j;
         rc = pthread_create(&threads[n], NULL, CalMatrix, (void *)&td[n]);
         if (rc){
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
         }
      }
   }

   for( i=0; i < NUM_THREADS; i++ ){
      rc = pthread_join(threads[i], &status);
      if (rc){
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
      cout << "Completed thread id :" << i ;
      cout << "  exiting with status :" << status << endl;
   }

   cout<<"Result:"<<endl;
   for( i = 0; i< M; i++){
      for( j = 0; j < N; j++){
         cout<<C[i][j]<<' ';
      }
      cout<<endl;
   }

   cout << "Main: program exiting." << endl;
   pthread_exit(NULL);
}