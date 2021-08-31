#include <inC2.h>

#include <iostream>
#include <string>
#include <unistd.h>

int main()
{
   InC2 *parent = new InC2();
   ChildProc *mult_child = parent->spawnMPIChild("multiplier 5");
   ChildProc *div_child =  parent->spawnMPIChild("divider 3");

   int val = 2;
   std::cout << "Starting with a value of " << val  << std::endl;
   std::cout << "The multiplier will win if the value goes over 50." << std::endl;
   std::cout << "The divider will win if the value goes below 1." << std::endl;


   while (val < 50 && val > 1)
   {
      std::cout << "\nPassing the current value to the multiplier." << std::endl;
      mult_child->input(std::to_string(val));
      val = stoi(mult_child->report().getPayload());
      std::cout << "Multiplier has reported a value of " << val << std::endl;

      std::cout << "Passing the current value to the divider." << std::endl;
      div_child->input(std::to_string(val));
      val = stoi(div_child->report().getPayload());
      std::cout << "Divider has reported a value of " << val << std::endl;
      std::cout << "Checking if one side has won the race." << std::endl;
   }

   if (val >= 50)
   {
      std::cout << "The multiplier has won the race!" << std::endl;
   }
   else
   {
      std::cout << "The divider has won the race!" << std::endl;
   }

   mult_child->stop();
   div_child->stop();

   delete parent;
   //sleep(5);
   return 0;
}
