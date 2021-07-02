#ifndef __CONFIG_H
#define __CONFIG_H






//____________________________________________________________________________
#include "scheduler.h"

  #define Scheduler_task_1_init     power_on_task_init
  #define Scheduler_task_1          power_on_task

  #define Scheduler_task_2_init     process_task_init
  #define Scheduler_task_2          process_task

  #define Scheduler_task_3_init     communic_task_init
  #define Scheduler_task_3          communic_command_task

  #define Scheduler_task_4_init     sensors_task_init
  #define Scheduler_task_4          sensors_task
  
  #define Scheduler_task_5_init     pannel_task_init
  #define Scheduler_task_5          pannel_task
//___________________________________________________________________/



#endif

