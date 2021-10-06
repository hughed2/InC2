#include "childProg.h"

#include <algorithm>
#include <cstring>
#include <iostream>

ChildProg::ChildProg(std::string command, int procs)
{
   _MPIChild(command, procs);
}

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

}

void ChildProg::sendMessage(Message msg, int rank)
{
   std::string text = msg.getText();
   int msgSize = text.size();
   MPI_Send(&msgSize, 1, MPI_INT, rank, 0, child_comm);
   MPI_Send(text.c_str(), msgSize, MPI_CHAR, rank, 0, child_comm);
}

Message ChildProg::receiveMessage(int rank)
{
   int msgSize;
   MPI_Recv(&msgSize, 1, MPI_INT, rank, 0, child_comm, MPI_STATUS_IGNORE);
   char* msg = (char *) malloc((1+msgSize) * sizeof(char));
   MPI_Recv(msg, msgSize, MPI_CHAR, rank, 0, child_comm, MPI_STATUS_IGNORE);
   msg[msgSize] = '\0';
   return Message(std::string(msg));
}

void ChildProg::stop()
{
   this->sendMessage(Message("STOP", ""));
}

Message ChildProg::report()
{
   this->sendMessage(Message("REPORT", ""));
   return this->receiveMessage();
}

void ChildProg::input(std::string payload)
{
   this->sendMessage(Message("INPUT", payload));
}

std::vector<double> ChildProg::receiveDoubles(int rank)
{
   int size;
   MPI_Request req;
   //MPI_Recv(&size, 1, MPI_INT, rank, 0, child_comm, MPI_STATUS_IGNORE);
   MPI_Irecv(&size, 1, MPI_INT, rank, 0, child_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   double* data = (double *) malloc((size) * sizeof(double));
   //MPI_Recv(data, size, MPI_DOUBLE, rank, 0, child_comm, MPI_STATUS_IGNORE);
   MPI_Irecv(data, size, MPI_DOUBLE, rank, 0, child_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   std::vector<double> data_vec(data, data+size);
   return data_vec;
}

//std::vector<double> ChildProg::receiveDoublesFromAll()
//{
//   int size;
//}

std::vector<int> ChildProg::receiveInts(int rank)
{  
   int size;
   MPI_Request req;
   //MPI_Recv(&size, 1, MPI_INT, rank, 0, child_comm, MPI_STATUS_IGNORE);
   MPI_Irecv(&size, 1, MPI_INT, rank, 0, child_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   int* data = (int *) malloc((size) * sizeof(int));
   //MPI_Recv(data, size, MPI_INT, rank, 0, child_comm, MPI_STATUS_IGNORE);
   MPI_Irecv(data, size, MPI_INT, rank, 0, child_comm, &req);
   MPI_Wait(&req, MPI_STATUS_IGNORE);
   std::vector<int> data_vec(data, data+size);
   return data_vec; 
}  

