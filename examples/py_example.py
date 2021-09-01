import InC2

# Note that even though this is a python script, we can use both python and C++ children
parent = InC2.InC2()
mult_child = parent.spawnMPIChild("python multiplier.py 5")
div_child = parent.spawnMPIChild("divider 3")

val = 2
print("Starting with a value of " + str(val))
print("The multiplier will win if the value goes over 50.")
print("The divider will win if the value goes below 1.")

while val < 50 and val > 1:
   print("\nPassing the current value to the multiplier.")
   mult_child.input(str(val))
   val = int(mult_child.report().getPayload())
   print("Multiplier has reported a value of " + str(val))

   print("Passing the current value to the divider.")
   div_child.input(str(val))
   val = int(div_child.report().getPayload())
   print("Divider has reported a value of " + str(val))
   print("Checking if one side has won the race.")


if val >= 50:
   print("The multiplier has won the race!")
else:
   print("The divider has won the race!")

mult_child.stop()
div_child.stop()

del parent

