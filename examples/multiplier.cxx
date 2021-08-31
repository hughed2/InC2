#include <iostream>
#include <mpi.h>

#include <inC2.h>

int main(int argc, char** argv)
{
   InC2 *parent = new InC2();

   int val = atoi(argv[1]);
   int retVal = 0;
   std::cout << "Multiplier will multiply all input by " << val << " and then return the result when requested." << std::endl;

   while(1)
   {
      Message msg = parent->receiveParentMessage();
      if (!msg.getCommand().compare("STOP"))
      {
         std::cout << "Multiplier received STOP command. Leaving now" << std::endl;
         break;
      }
      else if (!msg.getCommand().compare("INPUT"))
      {
         int payload = stoi(msg.getPayload()); // we should be passing a number
         retVal = val * payload;
      }
      else if (!msg.getCommand().compare("REPORT"))
      {
         parent->sendParentMessage(std::to_string(retVal));
      }
   }

   delete parent;
   return 0;
}

