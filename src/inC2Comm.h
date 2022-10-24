#ifndef INC2COMM_H
#define INC2COMM_H

#include "message.h"

#include <mpi.h>
#include <vector>

class InC2Comm
{
   public:
      // Create an InC2 wrapper around an MPI communicator
      InC2Comm(MPI_Comm &communicator, int procs);

      // Send or receive a Message object from a process
      void sendMessage(Message msg, int rank = 0);
      Message* checkForMessage(int rank = 0);

      // Pass data along to either a specific rank, or to all processes within the communicator
      void sendDoubles(std::vector<double> &data, int rank = 0);
      void sendDoublesToAll(std::vector<double> &data);
      void sendInts(std::vector<int> &data, int rank = 0);
      void sendIntsToAll(std::vector<int> &data);
      void waitForAsync();

      // Get data back from a process, either a specific rank or concatenate data from all ranks
      std::vector<double> receiveDoubles(int rank = 0);
      std::vector<double> receiveDoublesFromAll();
      std::vector<int> receiveInts(int rank = 0);
      std::vector<int> receiveIntsFromAll();

      // Helper functions
      void stop();

      int checkError(bool display_error = true);

      int getNumProcs() {return this->ranks;};

   private:
      MPI_Comm mpi_comm; // the MPI communicator we wrap around
      int error_code; // Helps us figure out errors on recv
      int ranks; // The number of processes within the communicator

      // Helper functions so we can do an MPI_Irecv under the hood
      int _getSize(int rank);
      void _getData(void* data, int size, MPI_Datatype datatype, int rank);

      // Helper function so we can do an MPI_Isendv under the hood
      void _send(void* data, MPI_Datatype datatype, int size, int rank);

      std::vector<MPI_Request> req_list; // Keep track of our open send requests, which will get closed by waitForAsync()
};

#endif
