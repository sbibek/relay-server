import socket
import time

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
s.bind(("0.0.0.0", 2001))
s.listen(5)
connection, address = s.accept()

print(connection)
while True:
    d = connection.recv(1)
    print(d)
