#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <pthread.h>

extern void bt_a_();
extern void bt_b_();

int main(int argc, char *argv[]) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, (void *) &bt_a_, NULL);
    pthread_create(&t2, NULL, (void *) &bt_b_, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("FINISH!\n");

    return 0;
}