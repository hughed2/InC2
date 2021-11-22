#include "inC2.h"

#include <mpi.h>

InC2::InC2()
{
   // Because we use MPI, we need to have an MPI_Init() ahead of time
   // We want to handle it if the user hasn't, but if they have we shouldn't
   // do it a second time
   MPI_Initialized(&initialized);
   if(!initialized)
   {
      MPI_Init(NULL, NULL);
   }
}

InC2::~InC2()
{
   // If we did an MPI_Init in the constructor, we should MPI_Finalize as well
   int finalized = 0;
   MPI_Finalized(&finalized);
   if(!initialized && !finalized)
   {
      MPI_Finalize();
   }
}

// This is a wrapper around the ChildProg constructor to create a set of children using MPI
// Because ChildProg ONLY creates children via MPI at the moment, this is a very thin wrapper
// but might change. Please review ChildProg::ChildProg() for up to date info on what these arguments are
ChildProg* 
InC2::spawnMPIChild(std::string command, int procs)
{
   return new ChildProg(command, procs);
}

// This sends a report to an MPI parent process (typically a controller script).
// It only takes in a string, but then wraps it in a Message object
void
InC2::sendParentMessage(std::string report)
{
   report = Message("REPORT", report).getText();
   int msgSize = report.size();

   MPI_Comm parent_comm;
   MPI_Comm_get_parent(&parent_comm);
   MPI_Send(&msgSize, 1, MPI_INT, 0, 0, parent_comm); // send the size first so the recipient knows how big the file is
   MPI_Send(report.c_str(), msgSize, MPI_CHAR, 0, 0, parent_comm);
}

// Receive a Message back from a parent.
// This is blocking, but there'll soon be probe functionality to better check for messages
Message
InC2::receiveParentMessage()
{
   MPI_Comm parent_comm;
   MPI_Comm_get_parent(&parent_comm);
   int msgSize;
   MPI_Recv(&msgSize, 1, MPI_INT, 0, 0, parent_comm, MPI_STATUS_IGNORE); // we need to get the size first for our MPI_Recv
   char* msg = (char *) malloc((1+msgSize) * sizeof(char));
   MPI_Recv(msg, msgSize, MPI_CHAR, 0, 0, parent_comm, MPI_STATUS_IGNORE);
   msg[msgSize] = '\0';
   return Message(std::string(msg));
}


// This is a wrapper around Isend to simplify the interface.
// This should be used internally only.
void
InC2::_send(void* data, MPI_Datatype datatype, int size)
{
   MPI_Comm parent_comm = MPI_COMM_SELF;
   MPI_Request req;
   MPI_Comm_get_parent(&parent_comm);
   MPI_Isend(data, size, datatype, 0, 0, parent_comm, &req);
   req_list.push_back(req);
}

// Send an array of doubles to our parent. This is non-blocking.
// To complete the send, we eventually need to call waitForAsync().
// One waitForAsync() call is enough for multiple sendDoubles() or sendInts()
//
// Note that we have to manually supply the size of the array
void
InC2::sendDoubles(double* data, int size)
{
   this->_send(data, MPI_DOUBLE, size);
}

// Send an array of ints to our parent. This is non-blocking.
// To complete the send, we eventually need to call waitForAsync().
// One waitForAsync() call is enough for multiple sendDoubles() or sendInts()
//
// Note that we have to manually supply the size of the array
void
InC2::sendInts(int* data, int size)
{
   this->_send(data, MPI_INT, size);
}

// This is a synchronous call that waits for all pending send requests to finish and
// frees up their buffers. You MUST eventually call this after a sendDoubles()
// or sendInts() or that memory will be permanently allocated and lost. 
void
InC2::waitForAsync()
{
   MPI_Status *statuses = (MPI_Status *) malloc(req_list.size() * sizeof(MPI_Status));
   MPI_Waitall(req_list.size(), req_list.data(), statuses);
   req_list.clear(); // We've gone through the iterations, we don't need this anymore
}
