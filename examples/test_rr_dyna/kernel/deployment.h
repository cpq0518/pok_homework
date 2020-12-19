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

#include <stdlib.h>
#ifndef __POK_KERNEL_GENERATED_DEPLOYMENT_H_
#define __POK_KERNEL_GENERATED_DEPLOYMENT_H_ 
#include <core/schedvalues.h>
#define POK_NEEEDS_DEBUG 1

#define POK_NEEDS_THREADS      1
#define POK_NEEDS_PARTITIONS   1
#define POK_NEEDS_SCHED        1
#define POK_NEEDS_TIME         1
#define POK_NEEDS_DEBUG        1
#define POK_NEEDS_CONSOLE      1
#define POK_NEEDS_LOCKOBJECTS  1

#define POK_CONFIG_NB_THREADS       8
#define POK_CONFIG_NB_LOCKOBJECTS       1
#define POK_CONFIG_NB_PARTITIONS    2

#define POK_CONFIG_PARTITIONS_SIZE  {120 * 1024, 120 * 1024};
#define POK_CONFIG_PARTITIONS_SCHEDULER {POK_SCHED_RR,POK_SCHED_EDF}
#define POK_CONFIG_PROGRAM_NAME  {"pr1/pr1.elf","pr2/pr2.elf"};

// #define POK_CONFIG_SCHEDULING_SLOTS {2000000000, 40000000000, 1000000000, 10000000000}   // 1~40 * 10e9 global var: pok_config_scheduling_slots 
// #define POK_CONFIG_SCHEDULING_MAJOR_FRAME 53000000000
// #define POK_CONFIG_SCHEDULING_SLOTS_ALLOCATION {0,1,0,1}  // 0~1 global var: pok_config_scheduling_slots_allocation
// #define POK_CONFIG_SCHEDULING_NBSLOTS 4  // 3~8: global var: pok_config_scheduling_nbslots

// 2020.12.16 dyna pr
#define POK_CONFIG_SCHEDULING_NBSLOTS(a) {(a) = rand()%5+3;}  // 3~8: 

#define POK_CONFIG_SCHEDULING_SLOTS(a) {int cnt = sizeof((a)) / sizeof((a)[0]);for(int i=0;i<cnt;i++){(a)[i] = (rand()%39+1)*1000000000;}}
#define POK_CONFIG_SCHEDULING_MAJOR_FRAME(sum,array) {sum = 0;int cnt = sizeof((array)) / sizeof((array)[0]);for(int i=0;i<cnt;i++){sum += (array)[i];}} 

#define POK_CONFIG_SCHEDULING_SLOTS_ALLOCATION(a) {int cnt = sizeof((a)) / sizeof((a)[0]);for(int i=0;i<cnt;i++){a[i] = rand()%1;}}


#define POK_NEEDS_THREAD_SUSPEND 1
#define POK_NEEDS_THREAD_SLEEP 1

#define POK_CONFIG_PARTITIONS_NTHREADS  {3,3}
#define POK_CONFIG_PARTITIONS_NLOCKOBJECTS  {1,0}

typedef enum
{
   pok_part_identifier_pr1 = 0,
   pok_part_identifier_pr2 = 1
}pok_part_identifiers_t;

typedef enum
{
   pok_part_id_pr1 = 0,
   pok_part_id_pr2 = 1
}pok_part_id_t;

typedef enum
{
   pok_nodes_node1 = 0
}pok_node_id_t;

#endif
