from PyInC2 import PyInC2

child = PyInC2().spawnChild("python hello_world_child.py", 1)

msg = child.checkForMessage()
while not msg:
   msg = child.checkForMessage()

print("Message received from child")

