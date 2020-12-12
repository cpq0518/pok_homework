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


#ifndef __POK_KERNEL_GENERATED_DEPLOYMENT_H_
#define __POK_KERNEL_GENERATED_DEPLOYMENT_H_ 

#define POK_NEEEDS_DEBUG 1

#define POK_NEEDS_THREADS      1
#define POK_NEEDS_PARTITIONS   1
#define POK_NEEDS_SCHED        1
#define POK_NEEDS_TIME         1
#define POK_NEEDS_DEBUG        1
#define POK_NEEDS_CONSOLE      1
#define POK_NEEDS_LOCKOBJECTS  1

#define POK_CONFIG_NB_THREADS       9//等于总共的threads数量(46行的3+2)再加2(一个main一个idle)
#define POK_CONFIG_NB_LOCKOBJECTS       1
#define POK_CONFIG_NB_PARTITIONS    2//总共定义了两个分区

#define POK_CONFIG_PARTITIONS_SIZE  {120 * 1024, 120 * 1024};
#define POK_CONFIG_PROGRAM_NAME  {"pr1/pr1.elf","pr2/pr2.elf"};

#define POK_CONFIG_SCHEDULING_SLOTS {2000000000, 40000000000, 1000000000, 10000000000}//一个周期：分区0先执行20000-->分区1再执行40000
                                                                                      //-->分区0再执行10000-->分区1再执行10000
#define POK_CONFIG_SCHEDULING_MAJOR_FRAME 53000000000//等于上面一行的总和
#define POK_CONFIG_SCHEDULING_SLOTS_ALLOCATION {0,1,0,1}//设计的分区的执行顺序是0-->1-->0-->1
#define POK_CONFIG_SCHEDULING_NBSLOTS 4//一个周期里有四段，对应38行

#define POK_NEEDS_THREAD_SUSPEND 1
#define POK_NEEDS_THREAD_SLEEP 1

#define POK_CONFIG_PARTITIONS_NTHREADS  {3,4}//分区0里3个线程，分区1里2个线程
#define POK_CONFIG_PARTITIONS_NLOCKOBJECTS  {1,0}

#define POK_NEEDS_SCHED_WEIGHTED_RR      1
#define POK_CONFIG_PARTITIONS_SCHEDULER {POK_SCHED_RR, POK_SCHED_WEIGHTED_RR}
//#define POK_CONFIG_PARTITIONS_SCHEDULER {POK_SCHED_RR,POK_SCHED_RR}

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
