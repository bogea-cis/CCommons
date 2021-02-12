#ifndef COBJECTCONTAINER_H
#define COBJECTCONTAINER_H

#include "clogger.h"

class CObjectContainer
{
public:
  static void initialize();

  static CLogger* getTraceInstance();

private:
  static CLogger* m_trace;
};

#endif /* COBJECTCONTAINER_H */
