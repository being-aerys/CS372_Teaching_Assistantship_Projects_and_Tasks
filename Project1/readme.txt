
Following is a commad to run the server on the server machine.
	
python Server.py 40000

Here, Server.py is the server program in python, 40000 is the port number passed as system argument and is going to be used by the client to connect to this machine.

Client.c is the client program. 

To compile the file, use gcc -o Client Client.c. This command means to output the compiled file to an executable "Client"


To run the client program, the user should provide a hostname and a port number on the command line. For example,
	./Client localhost 40000

Here, "Client" is the name of the executable clientside file, localhost is the hostname of the server to which the user wishes to connect (here we are using the same machine), and 40000 is the port number of the server to which we wish to connect.
