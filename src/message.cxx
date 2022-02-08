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


// Given a valid command (see above list), initialize our message
Message::Message(std::string command)
{
   if(!_isValidCommand(command))
   {
      std::cerr << "Error creating Message, invalid command: " << command << std::endl;
      return;
   }

   // Set up our basic structure, but don't worry about filling in any parameters yet
   this->j["command"] = command;
   this->j["metadata"] = json::object();
}

// Given a json object, initialize our message
Message::Message(json jsonObj)
{
   // We need to have a valid command
   this->j = jsonObj;
   if(!this->j.contains("command"))
   {
      std::cerr << "Error creating Message, no command included in json object" << std::endl;
   }

   std::string command = this->j["command"].get<std::string>();
   if(!_isValidCommand(command))
   {
      std::cerr << "Error creating Message, invalid command: " << command << std::endl;
   }

   // Make sure we always have metadata--but because it can be an empty object, it's okay if the user didn't supply it
   if(!this->j.contains("metadata"))
   {
      this->j["metadata"] = json::object();
   }

   // Make sure we don't have any extra objects
   if(!this->j.size() == 2)
   {
      std::cerr << "Error creating Message, invalid keys contained in json object" << std::endl;
   }
}

Message
stringToMessage(std::string jsonString)
{
   json j = json::parse(jsonString);
   return Message(j);
}

// Check to make sure that an input string is in the list of valid commands
bool
Message::_isValidCommand(std::string command)
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

// Get just the command value
std::string
Message::getCommand()
{
   return this->j["command"].get<std::string>();
}

// Convert the entire json into a string, so MPI actually knows what to do with it
std::string
Message::getString()
{
   return this->j.dump();
}

// Let us add parameters to the payload. As long as we aren't trying to serialize
// complex objects, the json library can handle it. So just create a few overloaded
// methods. Better than forcing the user to worry about templates--maybe later turn
// this into a macro.
void
Message::addIntParameter(std::string key, int value)
{
   this->j["metadata"][key] = value;
}

void
Message::addFloatParameter(std::string key, float value)
{
   this->j["metadata"][key] = value;
}

void
Message::addStringParameter(std::string key, std::string value)
{
   this->j["metadata"][key] = value;
}

// Remove a specified parameter from the metadata
void
Message::removeParameter(std::string key)
{
   this->j["metadata"].erase(key);
}

// Check to make sure a parameter exists
bool
Message::hasParameter(std::string key)
{
   return this->j["metadata"].contains(key);
}

// Return a parameter frm the metadata--we have to give these different names because we can't overload on return values
int
Message::getIntParameter(std::string key)
{
   return this->j["metadata"][key].get<int>();
}

float
Message::getFloatParameter(std::string key)
{
   return this->j["metadata"][key].get<float>();
}

std::string
Message::getStringParameter(std::string key)
{
   return this->j["metadata"][key].get<std::string>();
}

