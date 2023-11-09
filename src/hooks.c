#include <stdio.h>
#include <pthread.h>

void hook_onehz(void) {
    check_run();
}

/* 
void hook_gndreset(void) {
}
*/


/* This is at the beginning of the main routine, before usart and peripherals are inited */
void hook_init_early(void) {
}

/* This is at the end of the main routine */
void hook_init(void) {
}
