#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

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
      Message(std::string text);
      Message(std::string command, std::string payload);

      std::string getText(); // Returns the entire JSON text
      std::string getCommand(); // Returns the command string
      std::string getPayload(); // Returns the payload string if available

   private:
      std::string txt;
      std::string command;
      std::string payload; // optional depending on command type, JSON

      bool isValidCommand(std::string);
};

#endif
