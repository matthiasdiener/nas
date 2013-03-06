#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <omp.h>

int main(int argc, char *argv[]){
        int ret=0;
        int nthreads, tid;
        FILE *tf;

        omp_set_num_threads(16);

        #pragma omp parallel
        {
            /* Get thread number */

            tid = omp_get_thread_num();

                if(tid == 0){
                        printf("Hello World from thread = %d\n", tid);

                        // if((tf = fopen(argv[1], "a+")) == NULL){
                        //         printf("could not open gang file '%s'\n", argv[1]);
                        //         exit(-1);
                        // }

                        // fprintf(tf,"%d", syscall(SYS_gettid));
                        // fclose(tf);
                        omp_set_num_threads(4);
                        bt_a_();
                }
                else if(tid == 1){
                        printf("Hello World from thread = %d\n", tid);

                        // if((tf = fopen(argv[2], "a+")) == NULL){
                        //         printf("could not open gang file '%s'\n", argv[2]);
                        //         exit(-1);
                        // }

                        // fprintf(tf,"%d", syscall(SYS_gettid));
                        // fclose(tf);
                        omp_set_num_threads(4);
                        bt_b_();
                }
        }


        printf("FINISH!\n");

        return ret;
}