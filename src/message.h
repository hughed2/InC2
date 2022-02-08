#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "json.hpp"

using nlohmann::json;

extern std::string commandList[];

// A Message class is a simple JSON string that looks like the following:
// {
//    "command" : "<command>",
//    "payload" : "<payload>"
// }
//
// where <command> is any of "STOP", "RESTART", "RECONFIGURE", "REPORT", "INPUT"
// and <payload> is any valid json string (including blank).
//
// There is an implementation available that includes extra features like senderJobId,
// recipientJobId, timestamp, messageId, but we currently lack a valid use case for it
// so it's been removed for brevity.

class Message
{
   public:
      // Create a message. We can either supply an entire valid JSON (see above for definition)
      // or a valid command and payload and InC2 will construct the message automatically
      Message(std::string command);

      void addIntParameter(std::string key, int value);
      void addFloatParameter(std::string key, float value);
      void addStringParameter(std::string key, std::string value);

      void removeParameter(std::string key);

      int getIntParameter(std::string key);
      float getFloatParameter(std::string key);
      std::string getStringParameter(std::string key);

      std::string getCommand(); // Returns the command string
      std::string getString(); // Returns a full serialized string version of the json

   private:
      json j;

      bool _isValidCommand(std::string);
};

#endif

