#include "childProg.h"

#include <algorithm>
#include <cstring>
#include <iostream>


// Create a set of child processes. We pass in the command and the number of processes
// The command is executed as if it were on the command line. For example, if I run
// a job "hoMusic proj.in" on the command line, but I want to spawn 20, I could run
// ChildProg("hoMusic proj.in", 20)
// Note that this interface is set up as if we'll have multiple types of children in
// the future, but for now MPI is the only valid option
ChildProg::ChildProg(std::string command, int procs)
{
   _MPIChild(command, procs);
}

// Create MPI children. This is an internal function that shouldn't be called directly
// by a user. It will store a communicator (including only the children, NOT the calling process!)
// as well as the number of processes created
void
ChildProg::_MPIChild(std::string command, int procs)
{
   // MPI is a C lib, so we need to convert our C++ string
   size_t n = std::count(command.begin(), command.end(), ' ');
   char* cmd_c = (char*) malloc((1+command.size()) * sizeof(char)); // Add one for terminal \0
   strcpy(cmd_c, command.c_str());
   char** argv = (char**) malloc((1+n) * sizeof(char*));

   char* exe = strtok(cmd_c, " "); // This mangles cmd_c and uses already allocated memory, so don't need to free
   for(int i = 0; i < n; i++) {
      argv[i] = strtok(NULL, " ");
   }
   argv[n] = NULL; // MPI's list of args is null terminated

   MPI_Comm_spawn(cmd_c, argv, procs, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &child_comm, MPI_ERRCODES_IGNORE);

   free(argv);
   free(cmd_c);

   this->ranks = procs;
}

// Send a Message object to a child of a given rank
// Note that this is blocking
void ChildProg::sendMessage(Message msg, int rank)
{
   std::string text = msg.getText();
   int msgSize = text.size();
   MPI_Send(&msgSize, 1, MPI_INT, rank, 0, child_comm);
   MPI_Send(text.c_str(), msgSize, MPI_CHAR, rank, 0, child_comm);
}

// Receive a Message object from a child of a given rank
// Note that this is blocking
Message ChildProg::receiveMessage(int rank)
{
   int msgSize;
   MPI_Recv(&msgSize, 1, MPI_INT, rank, 0, child_comm, MPI_STATUS_IGNORE);
   char* msg = (char *) malloc((1+msgSize) * sizeof(char));
   MPI_Recv(msg, msgSize, MPI_CHAR, rank, 0, child_comm, MPI_STATUS_IGNORE);
   msg[msgSize] = '\0';
   return Message(std::string(msg));
}

// This is a helper function that just send a blocking STOP message to a child
void ChildProg::stop()
{
   this->sendMessage(Message("STOP", ""));
}

// This is a helper function that sends a blocking REPORT message to a child, and then waits for a response
Message ChildProg::report()
{
   this->sendMessage(Message("REPORT", ""));
   return this->receiveMessage();
}

// This is a helper function that lets us send new input (in string form) to a child
void ChildProg::input(std::string payload)
{
   this->sendMessage(Message("INPUT", payload));
}

// Use MPI_Probe to get the size of a message incoming from a specific rank so we can make a proper MPI_Irecv call
// This should be used internally only
int
ChildProg::_getSize(int rank)
{
   int size;
   MPI_Status status;
   MPI_Probe(rank, 0, child_comm, &status);
   MPI_Get_count(&status, MPI_INT, &size);
   return size;
}

// This is a wrapper around MPI_Irecv so we can easily loop through all of our ranks
// This should be used internally only
void
ChildProg::_getData(void* data, int size, MPI_Datatype datatype, int rank)
{
   MPI_Request req;
   MPI_Irecv(data, size, datatype, rank, 0, child_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
}

// Get a vector of doubles from a single rank of the child.
// This is a blocking call.
std::vector<double>
ChildProg::receiveDoubles(int rank)
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
ChildProg::receiveDoublesFromAll()
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
ChildProg::receiveInts(int rank)
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
ChildProg::receiveIntsFromAll()
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

