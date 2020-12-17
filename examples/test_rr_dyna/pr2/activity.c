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
#include <core/time.h>

void* pinger_job ()
{
   while (1)
   {
      printf("P2T1: begin of task1\n");

    //   pok_time_t tick;
    //   pok_time_gettick(&tick);
    //   printf ("P2T1: pok_time_gettick: ");
    //   printf ("%d\n", tick);

      pok_thread_sleep (5000000);
   }
}

void* pinger_job2 ()
{
    while (1)
    {
        printf("P2T2: begin of task2\n");

        // pok_time_t ns;
        // pok_time_get(&ns);
        // printf ("P2T2: pok_time_get: ");
        // printf ("%d\n", ns);

        pok_thread_sleep (5000000);
    }
}

