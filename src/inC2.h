#ifndef INC2_H
#define INC2_H

#include "childProg.h"
#include "message.h"

#include <mpi.h>
#include <vector>

class InC2
{
   public:
      InC2();
      ~InC2();

      // A wrapper around ChildProg--see that method for more details
      ChildProg* spawnMPIChild(std::string command, int procs = 1);

      // Send and Receive messages with our parent process
      void sendParentMessage(std::string report);
      Message receiveParentMessage();

      // Send a raw list asynchonously. After calling sendDoubles or sendInts, waitForAsync() must eventually be called
      void sendDoubles(double* data, int size);
      void sendInts(int* data, int size);
      void waitForAsync();

      // Helper functions so we can do an MPI_Isendv under the hood
      void _send(void* data, MPI_Datatype datatype, int size);

   private:
      int initialized = -1; // This is to help figure out whether we need to MPI_Finalize() or leave it to the user

      std::string jobId; // Preliminary functionality is in place for jobs to have job IDs, but is unused. This will need to get fleshed out use cases or removed
      std::string parentJobId = "NULL";

      int proc; // Unused

      std::vector<MPI_Request> req_list; // Keep track of our open send requests, which will get closed by waitForAsync()
};

#endif
