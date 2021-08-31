#include "inC2.h"

#include <algorithm> // std::count
#include <cstring>
#include <iostream>
#include <stdlib.h>

// Note: this should really be done as part of a JSON library, but it's easier this way for rapid prototyping and 0 dependencies on first pass
Message::Message(std::string text)
{
   this->txt = text;
   int b_pos = text.find("\"command\"") + 12;
   int e_pos = text.find("\"", b_pos);
   this->command = text.substr(b_pos, e_pos-b_pos);

   b_pos = text.find("\"payload\"") + 12;
   e_pos = text.find("\"", b_pos);
   this->payload = text.substr(b_pos, e_pos-b_pos);
   
}

Message::Message(std::string command, std::string payload)
{
   if(!isValidCommand(command))
   {
      std::cerr << "Invalid command: " << command << std::endl;
      return;
   }

   // Now actually create and send the message
   this->txt = std::string("{\n   \"command\": \"") + command + std::string("\",\n") +
                  std::string("   \"payload\": \"") + payload + std::string("\"\n}");
}

bool
Message::isValidCommand(std::string command)
{
   // For now, this is the best way to check to make sure a command is valid
   int i = 0;
   while (commandList[i].compare("NULL"))
   {
      if (!commandList[i++].compare(command))
      {
         return true;
      }
   }
   return false;
}

std::string
Message::getText()
{
   return this->txt;
}

std::string
Message::getCommand()
{
   return this->command;
}

std::string
Message::getPayload()
{
   return this->payload;
}

ChildProc::ChildProc(std::string command, bool MPI=true)
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
ChildProc::_MPIChild(std::string command)
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

void ChildProc::sendMessage(Message msg, int rank=0)
{
   std::string text = msg.getText();
   int msgSize = text.size();
   MPI_Send(&msgSize, 1, MPI_INT, rank, 0, child_comm);
   MPI_Send(text.c_str(), msgSize, MPI_CHAR, rank, 0, child_comm);
}

Message ChildProc::receiveMessage(int rank=0)
{
   int msgSize;
   MPI_Recv(&msgSize, 1, MPI_INT, rank, 0, child_comm, MPI_STATUS_IGNORE);
   char* msg = (char *) malloc((1+msgSize) * sizeof(char));
   MPI_Recv(msg, msgSize, MPI_CHAR, rank, 0, child_comm, MPI_STATUS_IGNORE);
   msg[msgSize] = '\0';
   return Message(std::string(msg));
}

void ChildProc::stop()
{
   this->sendMessage(Message("STOP", ""));
}

Message ChildProc::report()
{
   this->sendMessage(Message("REPORT", ""));
   return this->receiveMessage();
}

void ChildProc::input(std::string payload)
{
   this->sendMessage(Message("INPUT", payload));
}

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

ChildProc* 
InC2::spawnMPIChild(std::string command)
{
   return new ChildProc(command);
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
