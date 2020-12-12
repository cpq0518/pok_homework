/*
 *                               POK header
 * 
 * The following file is a part of the POK project. Any modification should
 * made according to the POK licence. You CANNOT use this file or a part of
 * this file is this part of a file for your own project
 *
 * For more information on the POK licence, please see our LICENCE FILE
 *
 * Please follow the coding guidelines described in doc/CODING_GUIDELINES
 *
 *                                      Copyright (c) 2007-2009 POK team 
 *
 * Created by julien on Thu Jan 15 23:34:13 2009 
 */


#include <libc/stdio.h>
#include <core/thread.h>
#include <core/semaphore.h>
#include <types.h>

extern uint8_t sid;

void *pinger_job() {
    pok_ret_t ret;
    while (1) {
        printf("P2T1: begin of task1\n");
        printf("P2T1: I will signal semaphores11111111\n");
        ret = pok_sem_signal(sid);
        printf("P2T1: pok_sem_signal, ret=%d\n", ret);
        pok_thread_sleep(5000000);
    }
}

void *pinger_job2() {
    while (1) {
        printf("P2T2: begin of task2\n");
        pok_thread_sleep(2000000);
    }
}

void *pinger_job3() {
    pok_ret_t ret;
    while (1) {
        printf("P2T3: begin of task3\n");
        printf("P2T3: I will wait for the semaphores33333333333333333333333333333333333\n");
        ret = pok_sem_wait(sid, 0);
        printf("P2T3: pok_sem_wait, ret=%d\n", ret);
        pok_thread_sleep(1000000);
    }
}

