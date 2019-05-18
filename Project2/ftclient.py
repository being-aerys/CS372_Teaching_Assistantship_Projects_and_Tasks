from socket import *
import sys
class Socket_Class:
    def __init__(self, server_IP, server_port):
        self.server_port_no = server_port
        self.socket = socket(AF_INET, SOCK_STREAM)
        self.server_ip = server_IP
        self.msg_to_forward = None
        self.msg_to_retrieve = None
        self.socket.connect((self.server_ip, self.server_port_no))

    def to_send(self, new_message):
        self.msg_to_forward = new_message
        self.socket.send(self.msg_to_forward)

    def to_receive(self):
        self.msg_to_retrieve = self.socket.recv(1024)
        return self.msg_to_retrieve

    def close(self):
        self.socket.shutdown(SHUT_RDWR)
        self.socket.close()

if __name__ == "__main__":
    server_name = sys.argv[1]
    server_port = int(sys.argv[2])
    file_port = int(sys.argv[-1])
    message = ""
    socket_to_instruct = Socket_Class(server_name, server_port)

    for instruction in sys.argv[3:]:
        message = message + '  ' + instruction

    socket_to_instruct.to_send(message)
    socket_to_instruct.to_receive()

    if socket_to_instruct.msg_to_retrieve == "ready":
        #for retrieval
        if sys.argv[3] == "-g":

            socket_for_data = Socket_Class(server_name, file_port)
            file = sys.argv[-2]
            print('Retrieving "' + file + '" from ' + server_name + ": " + str(file_port))
            file_next = open(file, "w+")
            while receive_until_arrives:
                file_next.write(receive_until_arrives)
                receive_until_arrives = socket_for_data.to_receive()
            file_next.close()
            print("Requested file retrieved.")
        #for listing
        if sys.argv[3] == "-l":

            socket_for_data = Socket_Class(server_name, file_port)
            print("List of data requested from " + server_name + ": " + str(file_port))
            print(socket_for_data.to_receive())


    else:
        print("\nError in retrieval.")

    socket_to_instruct.close()
    exit(0)
