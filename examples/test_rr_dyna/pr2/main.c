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
#include <core/partition.h>
#include <types.h>
#include "activity.h"
#include <core/time.h>


// 因为我用了rand生成的数做一些运算得到这个值比较大的时间 但是一直报错 没办法只能这么搞了
uint64_t fake_rand(int rand_input)
{
  switch (rand_input)
  {
    case 2: return (uint64_t)2000000000; break;
    case 3: return (uint64_t)3000000000; break;
    case 4: return (uint64_t)4000000000; break;
    default: return (uint64_t)3500000000; break;
  }
}


int main ()
{
  uint8_t tid;
  uint8_t tid2;
  int ret;
  pok_thread_attr_t     tattr;

  // init some var in sched.c
  // init_var_POK_CONFIG_SCHEDULING_NBSLOTS();
  // init_big_random_array();
  // init_pok_sched_slots();
  // init_pok_sched_slots_allocation();

  tattr.priority = 42;
  tattr.entry = pinger_job;
  int rand_1 = rand() % 3 + 2;
  // printf("rand_1: %d\n", rand_1);
  tattr.arrive_time = fake_rand(rand_1) ;   // 第2~4秒到达 因为怕等待时间太长就设的范围比较小 而且超过5 printf会溢出展示效果不好（实际运行正常）
  tattr.state = 6; //POK_STATE_NOT_ARRIVED

  ret = pok_thread_create(&tid , &tattr);
  printf ("[P2] thread create (1) returns=%d arrive_time=%u\n", ret, tattr.arrive_time);

  tattr.priority = 42;
  tattr.entry = pinger_job2;
  int rand_2 = rand() % 3 + 2;
  tattr.arrive_time = fake_rand(rand_2) ;   // 第2~4秒到达 因为怕等待时间太长就设的范围比较小 而且超过5 printf会溢出展示效果不好（实际运行正常）
  tattr.state = 6; //POK_STATE_NOT_ARRIVED

  ret = pok_thread_create(&tid2 , &tattr);
  printf ("[P2] thread create (2) returns=%d arrive_time=%u\n", ret, tattr.arrive_time);

  pok_partition_set_mode (POK_PARTITION_MODE_NORMAL);
  pok_thread_wait_infinite ();

  return (1);
}


