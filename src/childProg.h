#ifndef CHILDPROG_H
#define CHILDPROG_H

#include "message.h"

#include <mpi.h>

class ChildProg
{
   public:
      ChildProg(std::string, bool = true);

      void sendMessage(Message, int);
      Message receiveMessage(int);

      // These are just convenience wrappers to sendMessage
      void stop();
      Message report();
      void input(std::string);

   private:
      void _MPIChild(std::string);

      MPI_Comm child_comm;
      int* err_codes;
};

#endif
