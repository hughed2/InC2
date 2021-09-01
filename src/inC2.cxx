#include "inC2.h"

#include <mpi.h>

InC2::InC2()
{
   int initialized = 0;
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
   if(!finalized)
   {
      MPI_Finalize();
   }
}

ChildProg* 
InC2::spawnMPIChild(std::string command)
{
   return new ChildProg(command);
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

