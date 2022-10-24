from PyInC2 import PyInC2
from PyInC2 import Message

parent = PyInC2().getParent()
print("Child spawned, sending message to parent")

msg = Message("REPORT")
parent.sendMessage(msg)

