#ifndef INC2_H
#define INC2_H

#include <mpi.h>
#include <string>

std::string commandList[] = {
   "STOP",
   "RESTART",
   "RECONFIGURE",
   "REPORT",
   "INPUT",
   "NULL"
};

class Message
{
   public:
      Message(std::string);
      Message(std::string, std::string);

      std::string getText();
      std::string getCommand();
      std::string getPayload();

   private:
      std::string txt, command, payload;
      bool isValidCommand(std::string);
};

class ChildProc
{
   public:
      ChildProc(std::string, bool);

      void sendMessage(Message, int);
      Message receiveMessage(int);

      // These are just convenience wrappers to sendMessage
      void stop();
      Message report();
      void input(std::string);

   private:
      void _MPIChild(std::string);

      MPI_Comm child_comm;
      int* err_codes;
};

class InC2
{
   public:
      InC2();
      ~InC2();

      ChildProc* spawnMPIChild(std::string);
      void sendParentMessage(std::string);
      Message receiveParentMessage();
};

#endif
