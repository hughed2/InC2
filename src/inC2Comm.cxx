#include "inC2Comm.h"

// Create a communicator that can pass data back and forth between
// this process and one or more other processes
InC2Comm::InC2Comm(MPI_Comm &communicator)
{
   MPI_Comm_dup(communicator, &(this->mpi_comm)); // This gives us a new context for the communicator, making sure nothing is deleted or changed without our knowledge
   MPI_Comm_size(this->mpi_comm, &(this->ranks));
}

// Send a Message object to a process of a given rank
// Note that this is blocking
void
InC2Comm::sendMessage(Message msg, int rank)
{
   std::string text = msg.getString();
   int msgSize = text.size();
   MPI_Send(&msgSize, 1, MPI_INT, rank, 0, this->mpi_comm);
   MPI_Send(text.c_str(), msgSize, MPI_CHAR, rank, 0, this->mpi_comm);
}

// Check to see if there's a message from our communicator, and return if so.
// Otherwise, return void.
// Note that this is blocking
Message*
InC2Comm::checkForMessage(int rank)
{
   int flag;
   MPI_Status status;
   MPI_Iprobe(rank, 0, this->mpi_comm, &flag, &status); // non-blocking check to see if we have a message
   Message* retVal = NULL;
   if(flag){
      int msgSize;
      MPI_Recv(&msgSize, 1, MPI_INT, rank, 0, this->mpi_comm, MPI_STATUS_IGNORE);
      char* msg = (char *) malloc((1+msgSize) * sizeof(char));
      MPI_Recv(msg, msgSize, MPI_CHAR, rank, 0, this->mpi_comm, MPI_STATUS_IGNORE);
      msg[msgSize] = '\0';
      retVal = new Message(std::string(msg));
   }
   return NULL;
}

// This is a wrapper around Isend to simplify the interface.
// This should be used internally only.
void
InC2Comm::_send(void* data, MPI_Datatype datatype, int size, int rank)
{
   MPI_Request req;
   MPI_Isend(data, size, datatype, rank, 0, this->mpi_comm, &req);
   req_list.push_back(req);
}

// Send an array of doubles to one rank of our communicator. This is non-blocking.
// To complete the send, we eventually need to call waitForAsync().
// One waitForAsync() call is enough for multiple sendDoubles() or sendInts()
void
InC2Comm::sendDoubles(std::vector<double> &data, int rank)
{
   this->_send(data.data(), MPI_DOUBLE, data.size(), rank);
}

// This is a helper function to call sendDoubles to all ranks
// We still need to use waitForAsync().
void
InC2Comm::sendDoublesToAll(std::vector<double> &data)
{
   for (int i = 0; i < this->ranks; i++)
   {
      this->_send(data.data(), MPI_DOUBLE, data.size(), i);
   }
}

// Send an array of ints to one rank of our communicator. This is non-blocking.
// To complete the send, we eventually need to call waitForAsync().
// One waitForAsync() call is enough for multiple sendDoubles() or sendInts()
void
InC2Comm::sendInts(std::vector<int> &data, int rank)
{
   this->_send(data.data(), MPI_INT, data.size(), rank);
}

// This is a helper function to call sendInts to all ranks
// We still need to use waitForAsync().
void
InC2Comm::sendIntsToAll(std::vector<int> &data)
{
   for (int i = 0; i < this->ranks; i++)
   {
      this->_send(data.data(), MPI_INT, data.size(), i);
   }
}

// This is a synchronous call that waits for all pending send requests to finish and
// frees up their buffers. You MUST eventually call this after a sendDoubles()
// or sendInts() or that memory will be permanently allocated and lost. 
void
InC2Comm::waitForAsync()
{
   MPI_Status *statuses = (MPI_Status *) malloc(req_list.size() * sizeof(MPI_Status));
   MPI_Waitall(req_list.size(), req_list.data(), statuses);
   req_list.clear(); // We've gone through the iterations, we don't need this anymore
}


// Use MPI_Probe to get the size of a message incoming from a specific rank so we can make a proper MPI_Irecv call
// This should be used internally only
int
InC2Comm::_getSize(int rank)
{
   int size;
   MPI_Status status;
   MPI_Probe(rank, 0, this->mpi_comm, &status);
   MPI_Get_count(&status, MPI_INT, &size);
   return size;
}

// This is a wrapper around MPI_Irecv so we can easily loop through all of our ranks
// This should be used internally only
void
InC2Comm::_getData(void* data, int size, MPI_Datatype datatype, int rank)
{
   MPI_Request req;
   MPI_Irecv(data, size, datatype, rank, 0, this->mpi_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
}

// Get a vector of doubles from a single rank of the child.
// This is a blocking call.
std::vector<double>
InC2Comm::receiveDoubles(int rank)
{
   int size = this->_getSize(rank) / 2; // _getSize doesn't adjust for the fact that a double is twice the size of an int, so we do it here
   double* data = (double *) malloc((size) * sizeof(double));
   this->_getData(data, size, MPI_DOUBLE, rank);
   std::vector<double> data_vec(data, data+size);
   return data_vec;
}

// Get a vector of doubles from every child rank.
// These will be concatenated. For example, if you have three child processes [0,1,2] and they each send [x, y, z]
// you'll receive [x0, y0, z0, x1, y1, z1, x2, y2, z2], guaranteed in that order
// This is a blocking call.
std::vector<double>
InC2Comm::receiveDoublesFromAll()
{
   // We need to start out by getting the size each child is sending us. This is not guaranteed to be the same per child.
   int* sizes = (int *) malloc((this->ranks) * sizeof(int));
   int totalSize = 0;
   for(int i = 0; i < this->ranks; i++)
   {
      sizes[i] = this->_getSize(i) / 2; // _getSize doesn't adjust for the fact that a double is twice the size of an int, so we do it here
      totalSize += sizes[i];
   }

   // Now that we have the size, we can make an MPI_Irecv call for each child via _getData
   double* data = (double *) malloc((totalSize) * sizeof(double));
   double* dataCur = data;
   for(int i = 0; i < this->ranks; i++)
   {
      this->_getData(dataCur, sizes[i], MPI_DOUBLE, i);
      dataCur += sizes[i];
   }

   // We just have a standard array, so wrap it in a vector
   std::vector<double> data_vec(data, data+totalSize);
   return data_vec;
}

// Get a vector of ints from a single rank of the child.
// This is a blocking call.
std::vector<int>
InC2Comm::receiveInts(int rank)
{ 
   int size = this->_getSize(rank);
   int* data = (int *) malloc((size) * sizeof(int));
   this->_getData(data, size, MPI_INT, rank);
   std::vector<int> data_vec(data, data+size);
   return data_vec; 
}

// Get a vector of ints from every child rank.
// These will be concatenated. For example, if you have three child processes [0,1,2] and they each send [x, y, z]
// you'll receive [x0, y0, z0, x1, y1, z1, x2, y2, z2], guaranteed in that order
// This is a blocking call.
std::vector<int>
InC2Comm::receiveIntsFromAll()
{
   // We need to start out by getting the size each child is sending us. This is not guaranteed to be the same per child.
   int* sizes = (int *) malloc((this->ranks) * sizeof(int));
   int totalSize = 0;
   for(int i = 0; i < this->ranks; i++)
   {
      sizes[i] = this->_getSize(i);
      totalSize += sizes[i];
   }  

   // Now that we have the size, we can make an MPI_Irecv call for each child via _getData
   int* data = (int *) malloc((totalSize) * sizeof(int));
   int* dataCur = data;
   for(int i = 0; i < this->ranks; i++)
   {
      this->_getData(dataCur, sizes[i], MPI_INT, i);
      dataCur += sizes[i];
   }

   // We just have a standard array, so wrap it in a vector
   std::vector<int> data_vec(data, data+totalSize);
   return data_vec;
}

