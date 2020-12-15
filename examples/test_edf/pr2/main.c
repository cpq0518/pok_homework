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


#include <core/thread.h>
#include <core/partition.h>
#include <core/semaphore.h>
#include <types.h>
#include "activity.h"

uint8_t sid;

int main ()
{
  uint8_t tid;
  uint8_t tid2;
  uint8_t tid3;

  int ret;
  pok_thread_attr_t     tattr;

  ret = pok_sem_create(&sid , 0, 50, POK_SEMAPHORE_DISCIPLINE_FIFO);
  printf("[P2] pok_sem_create return=%d, mid=%d\n", ret, sid);

  tattr.priority = 42;
  tattr.entry = pinger_job;
  tattr.deadline = 80000000000;
 
//  tattr.period = 5000000000;
//  tattr.time_capacity = 2;

  ret = pok_thread_create(&tid , &tattr);
  printf ("[P2] thread create (1) returns=%d\n", ret);

  tattr.priority = 40;
  tattr.entry = pinger_job2;
  tattr.deadline = 70000000000;

// tattr.period = 5000000000;
// tattr.time_capacity = 2;
  ret = pok_thread_create(&tid2 , &tattr);
  printf ("[P2] thread create (2) returns=%d\n", ret);

  tattr.priority = 41;
  tattr.entry = pinger_job3;
  tattr.deadline = 90000000000;

// tattr.period = 5000000000;
// tattr.time_capacity = 2;
  ret = pok_thread_create(&tid3 , &tattr);
  printf ("[P2] thread create (3) returns=%d\n", ret);

  pok_partition_set_mode (POK_PARTITION_MODE_NORMAL);
  pok_thread_wait_infinite ();

  return (1);
}


