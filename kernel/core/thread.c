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

/**
 * \file    core/thread.c
 * \author  Julien Delange
 * \date    2008-2009
 * \brief   Thread management in kernel
 */
//#include <stdlib.h>
#include <types.h>

#include <arch.h>
#include <core/debug.h>
#include <core/error.h>
#include <core/thread.h>
#include <core/sched.h>
#include <core/partition.h>
#include <core/time.h>

#include <core/instrumentation.h>

#ifdef POK_NEEDS_THREADS


/**
 * We declare an array of threads. The amount of threads
 * is fixed by the software developper and we add two theads
 *    - one for the kernel thread (this code)
 *    - one for the idle task
 */
uint16_t flag_wrr=0;
pok_thread_t		         pok_threads[POK_CONFIG_NB_THREADS];

extern pok_partition_t     pok_partitions[POK_CONFIG_NB_PARTITIONS];

#ifdef POK_NEEDS_SCHED_RMS

/**
 * This part of the code sort the threads according to their
 * periods. This part is dedicated to the RMS scheduling algorithm.
 */
void pok_thread_insert_sort(uint16_t index_low, uint16_t index_high)
{
    uint32_t i=index_high,j=0;
    pok_thread_t val;

    val=pok_threads[i];
    j=i-1;
    while ( j>= index_low && pok_threads[j].period > val.period)
    {
        pok_threads[j+1] = pok_threads[j];
        j--;
    }
    pok_threads[j+1]=val;
}
#endif


#ifdef POK_NEEDS_SCHED_PREEMPTIVE_PRIORITY

/**
 * This part of the code sort the threads according to their
 * prioritys. This part is dedicated to the preemptive priority scheduling algorithm.
 */
void pok_thread_preemptive_priority_sort(uint16_t index_low, uint16_t index_high)
{
    uint32_t i=index_high,j=0;
    pok_thread_t val;

    val=pok_threads[i];
    j=i-1;
    while ( j>= index_low && pok_threads[j].priority > val.priority)
    {
        pok_threads[j+1] = pok_threads[j];
        j--;
    }
    pok_threads[j+1]=val;
}
#endif

#ifdef POK_NEEDS_SCHED_PREEMPTIVE_EDF

/**
 * This part of the code sort the threads according to their
 * ddls. This part is dedicated to the preemptive edf scheduling algorithm.
 */
void pok_thread_preemptive_edf_sort(uint16_t index_low, uint16_t index_high)
{
   /* 2020.12.16 test gettick */
   //  pok_time_t * tick;
   //  pok_ret_t ret1;
   //  ret1 = pok_time_gettick(tick);
   //  printf ("main: pok_time_gettick: ");
   //  printf ("%d\n", ret1);



    uint32_t i=index_high,j=0;
    pok_thread_t val;

    val=pok_threads[i];
    j=i-1;
    while ( j>= index_low && pok_threads[j].deadline > val.deadline)
    {
        pok_threads[j+1] = pok_threads[j];
        j--;
    }
    pok_threads[j+1]=val;
}
#endif




#ifdef POK_NEEDS_SCHED_WEIGHTED_RR
/**
 * This part is dedicated to the Weighted RR scheduling algorithm.
 */

/*This function replaces % to prevent overflow.*/
uint32_t get_mod(uint32_t a, uint32_t b)
{
   uint32_t mod;
   //printf("beforeeeeeeeeeeeeeee\n");
   mod=a-(uint32_t)(a/b)*b;
   //printf("mod=%d-(uint64_t)(%d divided by %d)*%d\n",a,a,b,b);
   return mod;
}



uint64_t gcd_of_two_weights(uint64_t weight1, uint64_t weight2)
{
   uint64_t result=weight2;
   printf("ENTER gcd of two weights!\n");
   uint64_t mod_result=get_mod(weight1,weight2);
   printf("mod_result=%d\n",mod_result);
   while(get_mod(weight1,weight2)!=0){
      result=get_mod(weight1,weight2);
      weight1=weight2;
      weight2=result;
   }
   return result;
}

uint64_t gcd_of_array_weights(uint64_t *array_weights, uint16_t n)
{
   //uint64_t a=sizeof(array_weights) / sizeof(array_weights[0]);
   /*printf("ENTER gcd!\n");
   printf("n = %d\n", n);
   uint64_t i, result;
   for (i=0;i<n;i++){
      printf("array_weights[i] = %d\n",array_weights[i]);
   }*/
   printf("ENTER gcd!\n");
   uint64_t i, result;
   result=array_weights[0];
   //printf("result before loop = %d\n", result);
   for (i=1;i<n;i++){
      result=gcd_of_two_weights(result,array_weights[i]);
   }
   //result=1;
   printf("LEAVE gcd! gcd = %d\n",result);
   
   return result;
}

uint64_t max_of_array_weights(uint64_t *array_weights, uint16_t n)
{
   //uint64_t a=sizeof(array_weights) / sizeof(array_weights[0]);
   printf("ENTER MAX!\n");
   uint64_t i, result;
   result=array_weights[0];

   for (i=1;i<n;i++){
      if(result<array_weights[i]){
         result=array_weights[i];
      }
   }
   printf("LEAVE MAX! max = %d\n",result);
   return result;
}

uint64_t sum_of_array_weights(uint64_t *array_weights, uint16_t n)
{
   //uint64_t a=sizeof(array_weights) / sizeof(array_weights[0]);
   printf("ENTER SUM!\n");
   uint64_t i, sum;
   sum=0;

   for (i=0;i<n;i++){
      sum+=array_weights[i];
   }
   printf("LEAVE SUM! sum = %d\n",sum);
   return sum;
}




uint64_t pok_thread_determine_wrr(uint64_t max, uint32_t gcd, uint32_t *i, uint32_t *cw, uint32_t n)
{
   //*i=*i-(n+1);
   while(1){
      *i=get_mod((*i+1),n);//fix me: i start from where??
      if(*i==0){
         *cw=*cw-gcd;
         if(*cw<=0){
            *cw=max;
            if(*cw==0){
               printf("Error in getting current weight!\n");
               return 0;
            }
         }
      }
      if(pok_threads[*i].weight>=*cw){
         return *i;
      }
   }
}


void pok_thread_weighted_rr_sort(uint16_t index_low, uint16_t n, uint64_t *array_weights)
{
   uint64_t sum=sum_of_array_weights(array_weights,n);
   uint64_t max=max_of_array_weights(array_weights,n);
   uint64_t gcd=gcd_of_array_weights(array_weights,n);
   printf("sum=%d\n",sum);
   printf("max=%d\n",max);
   printf("gcd=%d\n",gcd);
   printf("\n");
   uint32_t i,k,cw;

   i=-1+index_low;//排序时要不要加上index_low???? (index_low=4,i=3)
   cw=0;
   printf("i=%d\n",i);

   for(k=0;k<sum;k++){
      printf("In %dth loop of void pok_thread_rr_sort\n",k+1);
      pok_thread_determine_wrr(max,gcd,&i,&cw,n);
      printf("In %dth loop: \n",k+1);
   }
}

#endif


/**
 * Initialize threads array, put their default values
 * and so on
 */
void pok_thread_init(void)
{
   uint32_t i;

#ifdef POK_NEEDS_PARTITIONS
   uint32_t total_threads;
   uint8_t  j;

   total_threads = 0;

   for (j = 0 ; j < POK_CONFIG_NB_PARTITIONS ; j++)
   {
      total_threads = total_threads + pok_partitions[j].nthreads;//计算每个分区中的总线程数
   }

#if defined (POK_NEEDS_DEBUG) || defined (POK_NEEDS_ERROR_HANDLING)
   if (total_threads != (POK_CONFIG_NB_THREADS - 2))//等于才是正确的情况
   {
#ifdef POK_NEEDS_DEBUG
      printf ("Error in configuration, bad number of threads\n");
#endif
#ifdef POK_NEEDS_ERROR_HANDLING
      pok_kernel_error (POK_ERROR_KIND_KERNEL_CONFIG);
#endif
   }
#endif
#endif
   //KERNEL_THREAD的下标是所有index的倒数第二位，排在应用threads后面一位
   pok_threads[KERNEL_THREAD].priority	   = pok_sched_get_priority_min(0);//return 0
   pok_threads[KERNEL_THREAD].base_priority	   = pok_sched_get_priority_min(0);
   pok_threads[KERNEL_THREAD].state		   = POK_STATE_RUNNABLE;
   pok_threads[KERNEL_THREAD].next_activation = 0;
   
   //IDLE_THREAD的下标是所有index的最后一位，排在应用threads后面两位
   pok_threads[IDLE_THREAD].period                     = 0;
   pok_threads[IDLE_THREAD].deadline                   = 0;
   pok_threads[IDLE_THREAD].weight                     = 1;//self-adding
   pok_threads[IDLE_THREAD].time_capacity              = 0;
   pok_threads[IDLE_THREAD].next_activation            = 0;
   pok_threads[IDLE_THREAD].remaining_time_capacity    = 0;
   pok_threads[IDLE_THREAD].wakeup_time		       = 0;
   pok_threads[IDLE_THREAD].entry		       = pok_arch_idle;
   pok_threads[IDLE_THREAD].base_priority		       = pok_sched_get_priority_min(0);
   pok_threads[IDLE_THREAD].state		       = POK_STATE_RUNNABLE;

   pok_threads[IDLE_THREAD].sp			       = pok_context_create
                                                   (IDLE_THREAD,								             IDLE_STACK_SIZE,
						   (uint32_t)pok_arch_idle);

   for (i = 0; i < POK_CONFIG_NB_THREADS; ++i)
   {
      pok_threads[i].period                     = 0;
      pok_threads[i].deadline                   = 0;
      pok_threads[i].time_capacity              = 0;
      pok_threads[i].remaining_time_capacity    = 0;
      pok_threads[i].next_activation            = 0;
      pok_threads[i].wakeup_time                = 0;
      pok_threads[i].weight                     = 0;//self-adding
      pok_threads[i].state                      = POK_STATE_STOPPED;
  }
}

#ifdef POK_NEEDS_PARTITIONS
/**
 * Create a thread inside a partition
 * Return POK_ERRNO_OK if no error.
 * Return POK_ERRNO_TOOMANY if the partition cannot contain
 * more threads.
 */
pok_ret_t pok_partition_thread_create (uint32_t*                  thread_id,
                                       const pok_thread_attr_t*   attr,
                                       const uint8_t              partition_id)
{
   uint32_t id;
   uint32_t stack_vaddr;
   /**
    * We can create a thread only if the partition is in INIT mode
    */
   if (  (pok_partitions[partition_id].mode != POK_PARTITION_MODE_INIT_COLD) &&
         (pok_partitions[partition_id].mode != POK_PARTITION_MODE_INIT_WARM) )
   {
      return POK_ERRNO_MODE;
   }

   if (pok_partitions[partition_id].thread_index >= pok_partitions[partition_id].thread_index_high)
   {
#ifdef POK_NEEDS_ERROR_HANDLING
      POK_ERROR_CURRENT_PARTITION (POK_ERROR_KIND_PARTITION_CONFIGURATION);
#endif
      return POK_ERRNO_TOOMANY;
   }

   id = pok_partitions[partition_id].thread_index_low +  pok_partitions[partition_id].thread_index;
   pok_partitions[partition_id].thread_index =  pok_partitions[partition_id].thread_index + 1;

    if ((attr->priority <= pok_sched_get_priority_max (pok_partitions[partition_id].sched)) && (attr->priority >= pok_sched_get_priority_min (pok_partitions[partition_id].sched)))
   {
      pok_threads[id].priority      = attr->priority;
      pok_threads[id].base_priority      = attr->priority;
   }

   if (attr->period > 0)
   {
      pok_threads[id].period          = attr->period;
      pok_threads[id].next_activation = attr->period;
   }

   if (attr->deadline > 0)
   {
      pok_threads[id].deadline      = attr->deadline;
   }

   if (attr->weight > 0)
   {
      pok_threads[id].weight      = attr->weight;
   }

   // 2020.12.16 dyna
   if (attr->arrive_time > 0)
   {
      pok_threads[id].arrive_time      = attr->arrive_time;
   }
   else
   {
      pok_threads[id].arrive_time      = 0;  // 0: arrive_time is not set
   }

   if (attr->state != 0)
   {
      pok_threads[id].state      = attr->state;
   }

   if (pok_threads[id].arrive_flag != 1)
   {
      pok_threads[id].arrive_flag         = 0;
   }

   // END DYNA
   

#ifdef POK_NEEDS_SCHED_HFPPS
   pok_threads[id].payback = 0;
#endif /* POK_NEEDS_SCHED_HFPPS */

   if (attr->time_capacity > 0)
   {
      pok_threads[id].time_capacity = attr->time_capacity;
      pok_threads[id].remaining_time_capacity = attr->time_capacity;
   }
   else
   {
      pok_threads[id].remaining_time_capacity   = POK_THREAD_DEFAULT_TIME_CAPACITY;
      pok_threads[id].time_capacity             = POK_THREAD_DEFAULT_TIME_CAPACITY;
   }

   stack_vaddr = pok_thread_stack_addr (partition_id, pok_partitions[partition_id].thread_index);

   pok_threads[id].state		   = POK_STATE_RUNNABLE;
   pok_threads[id].wakeup_time   = 0;
   pok_threads[id].sp		      = pok_space_context_create (partition_id,
                                                             (uint32_t)attr->entry,
                                                             stack_vaddr,
                                                             0xdead,
                                                             0xbeaf);
   /*
    *  FIXME : current debug session about exceptions-handled
   printf ("thread sp=0x%x\n", pok_threads[id].sp);
   printf ("thread stack vaddr=0x%x\n", stack_vaddr);
   */
   pok_threads[id].partition        = partition_id; 
   pok_threads[id].entry            = attr->entry;
   pok_threads[id].init_stack_addr  = stack_vaddr;
   *thread_id = id;// thread_id由原来的relative to partition变成整个数组（多个分区)里的Index

#ifdef POK_NEEDS_SCHED_RMS
   if ((pok_partitions[partition_id].sched == POK_SCHED_RMS) && (id > pok_partitions[partition_id].thread_index_low))
   {
      pok_thread_insert_sort(pok_partitions[partition_id].thread_index_low+1,id);
   }
#endif

#ifdef POK_NEEDS_SCHED_PREEMPTIVE_PRIORITY
   if ((pok_partitions[partition_id].sched == POK_SCHED_PREEMPTIVE_PRIORITY) && (id > pok_partitions[partition_id].thread_index_low))
   {
      pok_thread_preemptive_priority_sort(pok_partitions[partition_id].thread_index_low+1,id);
   }
#endif

#ifdef POK_NEEDS_SCHED_PREEMPTIVE_EDF
   if ((pok_partitions[partition_id].sched == POK_SCHED_PREEMPTIVE_EDF) && (id > pok_partitions[partition_id].thread_index_low))
   {
      pok_thread_preemptive_edf_sort(pok_partitions[partition_id].thread_index_low+1,id);
      printf("HAS ENTER ifdef POK_NEEDS_SCHED_PREEMPTIVE_EDF\n");
   }
#endif

#ifdef POK_NEEDS_SCHED_WEIGHTED_RR
   
   if ((pok_partitions[partition_id].sched == POK_SCHED_WEIGHTED_RR) && (id > pok_partitions[partition_id].thread_index_low))
   {//现在写的是一整个sequence的排序，是否要写加入一个新的thread时的动态变化？
      /*uint64_t *array_weights;
      array_weights=(uint64_t*)calloc(pok_partitions[partition_id].nthreads,sizeof(uint64_t));
      for(uint32_t i=0;i<pok_partitions[partition_id].nthreads;i++){
         array_weights[i]=pok_threads[pok_partitions[partition_id].thread_index_low+i].weight;
      }  */
      /*uint64_t array_weights[pok_partitions[partition_id].nthreads];
      printf("num of threads = %d\n",pok_partitions[partition_id].nthreads);
      printf("weights:\n");
      for(uint32_t i=0;i<pok_partitions[partition_id].nthreads;i++){
         array_weights[i]=pok_threads[pok_partitions[partition_id].thread_index_low+i].weight;
         //printf("%d\n",array_weights[i]);
         printf("%d\n",pok_threads[pok_partitions[partition_id].thread_index_low+i].weight);
         //array_weights[i]=pok_threads[i].weight;
         //printf("%d\n",pok_threads[i].weight);
      }
      printf("\n");
      pok_thread_weighted_rr_sort(pok_partitions[partition_id].thread_index_low, pok_partitions[partition_id].nthreads,array_weights,pok_threads);
      */
      //void pok_thread_weighted_rr_sort(uint16_t index_low, uint16_t index_high, uint16_t n, uint64_t *array_weights, pok_thread_t *array_threads)    
      //pok_thread_insert_sort(pok_partitions[partition_id].thread_index_low+1,id);
      flag_wrr=flag_wrr+1;
      printf("flag = %d\n",flag_wrr);
      //printf("nthreads = %d\n",pok_partitions[partition_id].nthreads);
      if(flag_wrr==pok_partitions[partition_id].nthreads-1){
         printf("HAS ENTER sorting for POK_NEEDS_SCHED_WEIGHTED_RR\n");
         
         uint64_t array_weights[pok_partitions[partition_id].nthreads-1];
         for(uint32_t i=0;i<pok_partitions[partition_id].nthreads-1;i++){
            array_weights[i]=pok_threads[pok_partitions[partition_id].thread_index_low+i+1].weight;
            /*
            why using pok_threads[pok_partitions[partition_id].thread_index_low+i+1 instead of pok_threads[pok_partitions[partition_id].thread_index_low+i? 
            --- because the one whose index is thread_index_low is the main thread, this main thread is created by ret = pok_sem_create in examples/test_wrr/pr2/main.c

            so in the next few lines, use pok_partitions[partition_id].nthreads-1 to express the number of threads that are truly running 

            */
            printf("%d\n",array_weights[i]);
            printf("\n");
         }
         
            //fix here
            printf("PARAMETER 1: pok_partitions[partition_id].thread_index_low +1= %d\n",pok_partitions[partition_id].thread_index_low+1);
            pok_thread_weighted_rr_sort(pok_partitions[partition_id].thread_index_low+1, pok_partitions[partition_id].nthreads-1,array_weights);
            printf("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu\n");
      }
      //pok_thread_weighted_rr_sort(pok_partitions[partition_id].thread_index_low, pok_partitions[partition_id].nthreads,array_weights,pok_threads);
      //printf("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu\n");
   }
#endif



#ifdef POK_NEEDS_INSTRUMENTATION
      pok_instrumentation_task_archi (id);
#endif

   return POK_ERRNO_OK;
}
#endif


/**
 * Start a thread, giving its entry call with \a entry
 * and its identifier with \a id
 */
void pok_thread_start(void (*entry)(), unsigned int id)
{
   (void) id;
   entry();
}

#ifdef POK_NEEDS_THREAD_SLEEP
pok_ret_t pok_thread_sleep (uint32_t us)
{
   uint64_t mytime;
   mytime = ((uint64_t)us)*1000 + POK_GETTICK();
   pok_sched_lock_current_thread_timed (mytime);
   pok_sched ();
   return POK_ERRNO_OK;
}
#endif

#ifdef POK_NEEDS_THREAD_SLEEP_UNTIL
pok_ret_t pok_thread_sleep_until (uint32_t us)
{
  pok_sched_lock_current_thread_timed (((uint64_t)us)*1000);
   pok_sched ();
   return POK_ERRNO_OK;
}
#endif

#if defined (POK_NEEDS_THREAD_SUSPEND) || defined (POK_NEEDS_ERROR_HANDLING)
pok_ret_t pok_thread_suspend (void)
{
   pok_sched_stop_self ();
   pok_sched ();
   return POK_ERRNO_OK;
}
#endif

#ifdef POK_NEEDS_ERROR_HANDLING
pok_ret_t pok_thread_restart (const uint32_t tid)
{
   /**
    * Reinit timing values
    */

   pok_threads[tid].remaining_time_capacity  = pok_threads[tid].time_capacity;
   pok_threads[tid].state                    = POK_STATE_WAIT_NEXT_ACTIVATION;
   pok_threads[tid].wakeup_time              = 0;

   /**
    * Newer solution for later improvements.
    *
    pok_space_context_restart (pok_threads[tid].sp, (uint32_t) pok_threads[tid].entry, pok_threads[tid].init_stack_addr);
    *
    */

   /**
    * At this time, we build a new context for the thread.
    * It is not the best solution but it works at this time
    */
   pok_threads[tid].sp		      = pok_space_context_create (pok_threads[tid].partition,
                                                             (uint32_t)pok_threads[tid].entry,
                                                             pok_threads[tid].init_stack_addr,
                                                             0xdead,
                                                             0xbeaf);

   return POK_ERRNO_OK;
}
#endif

pok_ret_t pok_thread_delayed_start (const uint32_t id, const uint32_t us)
{
  uint64_t ns = 1000*((uint64_t) us);
  if (POK_CURRENT_PARTITION.thread_index_low > id || POK_CURRENT_PARTITION.thread_index_high < id)
    return POK_ERRNO_THREADATTR;
  pok_threads[id].priority = pok_threads[id].base_priority;
  //reset stack
  pok_context_reset(POK_USER_STACK_SIZE,pok_threads[id].init_stack_addr);
  if ((long long)pok_threads[id].period == -1) //-1 <==> ARINC INFINITE_TIME_VALUE
    {
      if (pok_partitions[pok_threads[id].partition].mode == POK_PARTITION_MODE_NORMAL)
  	{
  	  if (ns == 0)
  	    {
  	      pok_threads[id].state = POK_STATE_RUNNABLE;
  	      pok_threads[id].end_time = POK_GETTICK() + pok_threads[id].time_capacity;
  	    }
  	  else
  	    {
  	      pok_threads[id].state = POK_STATE_WAITING;
  	      pok_threads[id].wakeup_time = POK_GETTICK() + ns;
  	    }
	  //the preemption is always enabled so
  	  pok_sched();
  	}
      else //the partition mode is cold or warm start
  	{
  	  pok_threads[id].state = POK_STATE_DELAYED_START;
	  pok_threads[id].wakeup_time = ns;
  	}
    }
  else
    {
      if (pok_partitions[pok_threads[id].partition].mode == POK_PARTITION_MODE_NORMAL)
	    { // set the first release point
	      pok_threads[id].next_activation = ns + POK_GETTICK();
	      pok_threads[id].end_time = pok_threads[id].deadline + pok_threads[id].next_activation;
	    }
      else
	   {
	     pok_threads[id].state = POK_STATE_DELAYED_START;
	     pok_threads[id].wakeup_time = ns; // temporarly storing the delay, see set_partition_mode
	   }
    }
  return POK_ERRNO_OK;
}

pok_ret_t pok_thread_get_status (const uint32_t id, pok_thread_attr_t *attr)
{
  if (POK_CURRENT_PARTITION.thread_index_low > id || POK_CURRENT_PARTITION.thread_index_high < id)
    return POK_ERRNO_PARAM;
  attr->deadline = pok_threads[id].end_time;
  attr->state = pok_threads[id].state;
  attr->priority = pok_threads[id].priority;
  attr->entry = pok_threads[id].entry;
  attr->period = pok_threads[id].period;
  attr->weight = pok_threads[id].weight;//self-adding
  attr->time_capacity = pok_threads[id].time_capacity;
  attr->stack_size = POK_USER_STACK_SIZE;
  return POK_ERRNO_OK;
}

pok_ret_t      pok_thread_set_priority (const uint32_t id, const uint32_t priority)
{
	if (POK_CURRENT_PARTITION.thread_index_low > id || POK_CURRENT_PARTITION.thread_index_high < id)
		return POK_ERRNO_PARAM;
	pok_threads[id].priority = priority;
	/* preemption is always enabled so ... */
	pok_sched();
	return POK_ERRNO_OK;
}

pok_ret_t pok_thread_resume (const uint32_t id)
{
	if (POK_CURRENT_PARTITION.thread_index_low > id || POK_CURRENT_PARTITION.thread_index_high < id)
		return POK_ERRNO_THREADATTR;
	pok_threads[id].wakeup_time = POK_GETTICK();
	pok_threads[id].state = POK_STATE_RUNNABLE;
	/* preemption is always enabled */
	pok_sched();
	return POK_ERRNO_OK;
}

pok_ret_t pok_thread_suspend_target (const uint32_t id)
{
	if (POK_CURRENT_PARTITION.thread_index_low > id || POK_CURRENT_PARTITION.thread_index_high < id || id == POK_SCHED_CURRENT_THREAD)
		return POK_ERRNO_THREADATTR;
	pok_threads[id].state = POK_STATE_STOPPED;
	return POK_ERRNO_OK;
}

#endif
