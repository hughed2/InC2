import InC2

import sys

parent = InC2.InC2()

val = int(sys.argv[1])
retVal = 0
print("Multiplier will multiply all input by " + str(val) + " and then return the result when requested.")

while True:
   msg = parent.receiveParentMessage();
   if msg.getCommand() == "STOP":
      print("Multiplier received STOP command. Leaving now")
      break
   elif msg.getCommand() == "INPUT":
      payload = int(msg.getPayload()) # we should be passing a number
      retVal = val * payload
   elif msg.getCommand() == "REPORT":
      parent.sendParentMessage(str(retVal))

del parent

