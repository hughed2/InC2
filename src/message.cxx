#include "message.h"

#include <iostream>

std::string commandList[] = {
   "STOP",
   "RESTART",
   "RECONFIGURE",
   "REPORT",
   "INPUT",
   "NULL"
};


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

