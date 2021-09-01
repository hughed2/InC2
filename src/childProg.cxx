#include "childProg.h"

#include <algorithm>
#include <cstring>
#include <iostream>

ChildProg::ChildProg(std::string command, bool MPI)
{
   if(MPI)
   {
      _MPIChild(command);
   }
   else
   {
      std::cerr << "Only MPI children are implemented for now. Note that a child that uses the InC2 class counts as an MPI child." << std::endl;
   }
}

void
ChildProg::_MPIChild(std::string command)
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

   MPI_Comm_spawn(cmd_c, argv, 1, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &child_comm, MPI_ERRCODES_IGNORE);

   free(argv);
   free(cmd_c);

}

void ChildProg::sendMessage(Message msg, int rank=0)
{
   std::string text = msg.getText();
   int msgSize = text.size();
   MPI_Send(&msgSize, 1, MPI_INT, rank, 0, child_comm);
   MPI_Send(text.c_str(), msgSize, MPI_CHAR, rank, 0, child_comm);
}

Message ChildProg::receiveMessage(int rank=0)
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


