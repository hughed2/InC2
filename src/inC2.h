#ifndef INC2_H
#define INC2_H

#include "inC2Comm.h"

#include <mpi.h>

// InC2 plays two roles. First, it's a factory for InC2Comm objects. Second, it lets us keep track of MPI initialization and finalization
// Forcing it into a class means the user never needs to worry about MPI if they aren't using it elsewhere.
// Other methods should probably be kept out of this class to keep loose coupling, but factory methods belong here
// In the future, we might want to allow other communication methods beyond MPI.
class InC2
{
   public:
      InC2(std::string mpi_args = std::string());
      ~InC2();

      InC2Comm* spawnChild(std::string command, int procs = 1, std::string hostfile = std::string()); // Use MPI to spawn a child program, and then return an InC2Comm pointing at it
      InC2Comm* getParent(); // This returns an InC2Comm pointed at the MPI parent

   private:
      int mpi_initialized = -1; // This is to help figure out whether we need to MPI_Finalize() or leave it to the user
};

#endif
