#include <iostream>
#include <mpi.h>

#include <inC2.h>

int main(int argc, char** argv)
{
   InC2 *parent = new InC2();

   int val = atoi(argv[1]);
   int retVal = 0;
   std::cout << "Divider will divide all input by " << val << " and then return the result when requested." << std::endl;

   while(1)
   {
      Message msg = parent->receiveParentMessage();
      if (!msg.getCommand().compare("STOP"))
      {
         std::cout << "Divider received STOP command. Leaving now" << std::endl;
         break;
      }
      else if (!msg.getCommand().compare("INPUT"))
      {
         int payload = stoi(msg.getPayload()); // we should be passing a number
         retVal = payload / val;
      }
      else if (!msg.getCommand().compare("REPORT"))
      {
         parent->sendParentMessage(std::to_string(retVal));
      }
   }

   delete parent;
   return 0;
}

