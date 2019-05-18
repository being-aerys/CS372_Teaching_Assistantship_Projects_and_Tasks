
import sys
from socket import *

#first, we start the server
def startUp(serverSocket):
    #pass the socket number as an argument
    serverPort = int(sys.argv[1])
    serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    serverSocket.bind(('',serverPort))
    serverSocket.listen(1)

    print("Server is up and listening now")

#this function reads any incoming message
def readMessage(readMsg, connectionSocket, MSGSIZE):
    readMsg = connectionSocket.recv(MSGSIZE)
    if readMsg:
        print ( readMsg)

    return readMsg

#this funtion is used to send a message to the client
def sendMessage(sendMsg, connectionSocket):
    handle = "HOST: "
    #Accepts message input from user
    sendMsg = raw_input(handle)
    sendMsg = "" + sendMsg

    #Sends user message to client.
    connectionSocket.send(sendMsg)

    return sendMsg


MSGSIZE = 1024
serverSocket = socket(AF_INET, SOCK_STREAM)

#starting the server to listen to client on socket.
startUp(serverSocket)

#loop executes while server is running
while True:
# This flag indicates whether the server has received an initial message
    flag = 0
    print ("Waiting for Client")
    connectionSocket, addr = serverSocket.accept()
    print ('TCP Connection has been created with:', addr)
    print

# Read initial message from client
    readMsg = connectionSocket.recv(MSGSIZE)

# This IF loop only executes to print the initial message received from the
# client.
    if (readMsg and flag == 0):
        flag = 1
        print (readMsg)

# while loop runs to read and write messages
    while True:

        #Function reads messages sent from client and prints to screen.
        readMsg = readMessage(readMsg, connectionSocket, MSGSIZE)

        if "quit" in readMsg:
            print ("closing the connection")
            break

        #Function sends messages from server to client and prints to screen.
        sendMsg = " "
        sendMsg = sendMessage(sendMsg, connectionSocket)
        if sendMsg == "HOST: \\quit":
            print ("closing the connection")
        #Break statement escapes inner loop and returns server to listen state.
            break

        continue


    connectionSocket.close()

