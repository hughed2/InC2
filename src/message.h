#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "json.hpp"

using nlohmann::json;

extern std::string commandList[];

// A Message class is a wrapper around a JSON object that looks like the following:
// {
//    "command" : "<command>",
//    "metadata" : "<payload>"
// }
//
// where <command> is any of "STOP", "RESTART", "RECONFIGURE", "REPORT", "INPUT"
// and <metadata> is a set of keys and parameters (of semi-arbitrary type).
//
// There is an implementation available that includes extra features like senderJobId,
// recipientJobId, timestamp, messageId, but we currently lack a valid use case for it
// so it's been removed for brevity.

class Message
{
   public:
      // All we need to create a Message is a valid command type, then we can construct the metadata after
      Message(std::string command);
      Message(json jsonObj);

      void addIntParameter(std::string key, int value);
      void addFloatParameter(std::string key, float value);
      void addStringParameter(std::string key, std::string value);

      void removeParameter(std::string key);

      bool hasParameter(std::string key);

      int getIntParameter(std::string key);
      float getFloatParameter(std::string key);
      std::string getStringParameter(std::string key);

      std::string getCommand(); // Returns the command string
      std::string getString(); // Returns a full serialized string version of the json

   private:
      json j;

      bool _isValidCommand(std::string);
};

Message* stringToMessage(std::string jsonString);

#endif

