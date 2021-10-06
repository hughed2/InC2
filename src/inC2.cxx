#include "inC2.h"

#include <mpi.h>

InC2::InC2()
{
   MPI_Initialized(&initialized);
   if(!initialized)
   {
      MPI_Init(NULL, NULL);
   }
}

InC2::~InC2()
{
   int finalized = 0;
   MPI_Finalized(&finalized);
   if(!initialized && !finalized) // only MPI Finalize if this is the object that initialized in the first place
   {
      MPI_Finalize();
   }
}

ChildProg* 
InC2::spawnMPIChild(std::string command, int procs)
{
   return new ChildProg(command, procs);
}

void
InC2::sendParentMessage(std::string report)
{
   report = Message("REPORT", report).getText();
   int msgSize = report.size();

   MPI_Comm parent_comm;
   MPI_Comm_get_parent(&parent_comm);
   MPI_Send(&msgSize, 1, MPI_INT, 0, 0, parent_comm);
   MPI_Send(report.c_str(), msgSize, MPI_CHAR, 0, 0, parent_comm);
}

Message
InC2::receiveParentMessage()
{
   MPI_Comm parent_comm;
   MPI_Comm_get_parent(&parent_comm);
   int msgSize;
   MPI_Recv(&msgSize, 1, MPI_INT, 0, 0, parent_comm, MPI_STATUS_IGNORE);
   char* msg = (char *) malloc((1+msgSize) * sizeof(char));
   MPI_Recv(msg, msgSize, MPI_CHAR, 0, 0, parent_comm, MPI_STATUS_IGNORE);
   msg[msgSize] = '\0';
   return Message(std::string(msg));
}

void
InC2::sendDoubles(double* data, int size)
{
   MPI_Comm parent_comm = MPI_COMM_SELF;
   MPI_Request req;
   MPI_Status stat;
   MPI_Comm_get_parent(&parent_comm);
   //MPI_Send(&size, 1, MPI_INT, 0, 0, parent_comm);
   MPI_Isend(&size, 1, MPI_INT, 0, 0, parent_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   //MPI_Request_free(&req);
   //MPI_Send(data, size, MPI_DOUBLE, 0, 0, parent_comm);
   MPI_Isend(data, size, MPI_DOUBLE, 0, 0, parent_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   //MPI_Request_free(&req);
}

void
InC2::sendInts(int* data, int size)
{
   MPI_Comm parent_comm = MPI_COMM_SELF;
   MPI_Request req;
   MPI_Status stat;
   MPI_Comm_get_parent(&parent_comm);
   //MPI_Send(&size, 1, MPI_INT, 0, 0, parent_comm);
   MPI_Isend(&size, 1, MPI_INT, 0, 0, parent_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   //MPI_Request_free(&req);
   //MPI_Send(data, size, MPI_INT, 0, 0, parent_comm);
   MPI_Isend(data, size, MPI_INT, 0, 0, parent_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   //MPI_Request_free(&req);
}

