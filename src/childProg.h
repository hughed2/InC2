#ifndef CHILDPROG_H
#define CHILDPROG_H

#include "message.h"

#include <mpi.h>
#include <vector>

class ChildProg
{
   public:
      ChildProg(std::string, int);

      void sendMessage(Message, int = 0);
      Message receiveMessage(int = 0);

      std::vector<double> receiveDoubles(int = 0);
      std::vector<double> receiveDoublesFromAll();
      std::vector<int> receiveInts(int = 0);
      std::vector<int> receiveIntsFromAll();

      int _getSize(int);
      void _getData(void*, int, MPI_Datatype, int);

      // These are just convenience wrappers to sendMessage
      void stop();
      Message report();
      void input(std::string);

   private:
      void _MPIChild(std::string, int);

      MPI_Comm child_comm;
      int* err_codes;

      std::string jobId;
      std::string parentJobId;
      int ranks;

};

#endif
