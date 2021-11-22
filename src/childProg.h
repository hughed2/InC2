#ifndef CHILDPROG_H
#define CHILDPROG_H

#include "message.h"

#include <mpi.h>
#include <vector>

class ChildProg
{
   public:
      // Create the child. Pass in the actual executable/args to run the child, as well as the number of procs
      ChildProg(std::string command, int procs);

      // Send or receive a Message object from the child
      void sendMessage(Message msg, int rank = 0);
      Message receiveMessage(int rank = 0);

      // Get data back from the child, either a specific rank or concatenate data from all ranks
      std::vector<double> receiveDoubles(int rank = 0);
      std::vector<double> receiveDoublesFromAll();
      std::vector<int> receiveInts(int rank = 0);
      std::vector<int> receiveIntsFromAll();

      // Helper functions so we can do an MPI_Irecv under the hood
      int _getSize(int rank);
      void _getData(void* data, int size, MPI_Datatype datatype, int rank);

      // These are just convenience wrappers to sendMessage
      void stop();
      Message report();
      void input(std::string payload);

   private:
      // Use MPICommSpawn to create a child
      void _MPIChild(std::string, int);

      MPI_Comm child_comm; // the MPI communicator for the children
      int* err_codes; // unused

      std::string jobId; // Preliminary functionality is in place for jobs to have job IDs, but is unused. This will need to get fleshed out use cases or removed
      std::string parentJobId;

      int ranks; // The number of processes of the child job

};

#endif
