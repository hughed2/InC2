#ifndef INC2_H
#define INC2_H

#include "childProg.h"
#include "message.h"

class InC2
{
   public:
      InC2();
      ~InC2();

      ChildProg* spawnMPIChild(std::string, int = 1);
      void sendParentMessage(std::string);
      Message receiveParentMessage();

      void sendDoubles(double*, int);
      void sendInts(int*, int);

   private:
      int initialized = -1;

      std::string jobId;
      std::string parentJobId = "NULL";
      int proc;
};

#endif
