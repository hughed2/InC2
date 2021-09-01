#ifndef INC2_H
#define INC2_H

#include "childProg.h"
#include "message.h"

class InC2
{
   public:
      InC2();
      ~InC2();

      ChildProg* spawnMPIChild(std::string);
      void sendParentMessage(std::string);
      Message receiveParentMessage();
};

#endif
