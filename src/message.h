#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

extern std::string commandList[];

class Message
{
   public:
      Message(std::string);
      Message(std::string, std::string);

      std::string getText();
      std::string getCommand();
      std::string getPayload();

   private:
      std::string txt;
      std::string command;
      std::string payload; // optional depending on command type, JSON

      bool isValidCommand(std::string);
};

#endif
