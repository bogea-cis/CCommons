/**
 *
 */

#include "config.h"
#include "scheduler.h"


/**
 *
 *///____________________
void scheduler_init( void)
{
   #ifdef Scheduler_task_1_init
      Scheduler_task_1_init();
   #endif
   #ifdef Scheduler_task_2_init
      Scheduler_task_2_init();
   #endif
   #ifdef Scheduler_task_3_init
      Scheduler_task_3_init();
   #endif
   #ifdef Scheduler_task_4_init
      Scheduler_task_4_init();
   #endif
   #ifdef Scheduler_task_5_init
      Scheduler_task_5_init();
   #endif
   #ifdef Scheduler_task_6_init
      Scheduler_task_6_init();
   #endif
   #ifdef Scheduler_task_7_init
      Scheduler_task_7_init();
   #endif
   #ifdef Scheduler_task_8_init
      Scheduler_task_8_init();
   #endif
   #ifdef Scheduler_task_9_init
      Scheduler_task_9_init();
   #endif
   #ifdef Scheduler_task_10_init
      Scheduler_task_10_init();
   #endif
   #ifdef Scheduler_task_11_init
      Scheduler_task_11_init();
   #endif
}

/**
 *
 *///_____________________
void scheduler_tasks( void)
{
   for(;;)
   {
      #ifdef Scheduler_task_1
         Scheduler_task_1();
      #endif
      #ifdef Scheduler_task_2
         Scheduler_task_2();
      #endif
      #ifdef Scheduler_task_3
         Scheduler_task_3();
      #endif
      #ifdef Scheduler_task_4
         Scheduler_task_4();
      #endif
      #ifdef Scheduler_task_5
         Scheduler_task_5();
      #endif
      #ifdef Scheduler_task_6
         Scheduler_task_6();
      #endif
      #ifdef Scheduler_task_7
         Scheduler_task_7();
      #endif
      #ifdef Scheduler_task_8
         Scheduler_task_8();
      #endif
      #ifdef Scheduler_task_9
         Scheduler_task_9();
      #endif
      #ifdef Scheduler_task_10
         Scheduler_task_10();
      #endif
      #ifdef Scheduler_task_11
         Scheduler_task_11();
      #endif
}

