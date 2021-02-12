#ifndef __SCHEDULER_H
#define __SCHEDULER_H




#ifdef Scheduler_task_1_init
  extern  void Scheduler_task_1_init (void);
#else
#ifdef Scheduler_task_2_init
  extern  void Scheduler_task_2_init (void);
#endif
#ifdef Scheduler_task_3_init
  extern  void Scheduler_task_3_init (void);
#endif
#ifdef Scheduler_task_4_init
  extern  void Scheduler_task_4_init (void);
#endif
#ifdef Scheduler_task_5_init
  extern  void Scheduler_task_5_init (void);
#endif
#ifdef Scheduler_task_6_init
  extern  void Scheduler_task_6_init (void);
#endif
#ifdef Scheduler_task_7_init
  extern  void Scheduler_task_7_init (void);
#endif
#ifdef Scheduler_task_8_init
  extern  void Scheduler_task_8_init (void);
#endif
#ifdef Scheduler_task_9_init
  extern  void Scheduler_task_9_init (void);
#endif
#ifdef Scheduler_task_10_init
  extern  void Scheduler_task_10_init (void);
#endif
#ifdef Scheduler_task_11_init
  extern  void Scheduler_task_11_init (void);
#endif

#ifdef Scheduler_task_1
  extern  void Scheduler_task_1 (void);
#else
#ifdef Scheduler_task_2
  extern  void Scheduler_task_2 (void);
#endif
#ifdef Scheduler_task_3
  extern  void Scheduler_task_3 (void);
#endif
#ifdef Scheduler_task_4
  extern  void Scheduler_task_4 (void);
#endif
#ifdef Scheduler_task_5
  extern  void Scheduler_task_5 (void);
#endif
#ifdef Scheduler_task_6
  extern  void Scheduler_task_6 (void);
#endif
#ifdef Scheduler_task_7
  extern  void Scheduler_task_7 (void);
#endif
#ifdef Scheduler_task_8
  extern  void Scheduler_task_8 (void);
#endif
#ifdef Scheduler_task_9
  extern  void Scheduler_task_9 (void);
#endif
#ifdef Scheduler_task_10
  extern  void Scheduler_task_10 (void);
#endif
#ifdef Scheduler_task_11
  extern  void Scheduler_task_11 (void);
#endif


void scheduler_init( void);
void scheduler_tasks( void);





#endif
