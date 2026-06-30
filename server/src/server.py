import socket
import datetime

# set the server address
SERVER_IP, SERVER_PORT = '127.0.0.1', 8080
SERVER_ADDRESS = (SERVER_IP, SERVER_PORT)

# init TCP server
SERVER_SOCKET = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# start server
SERVER_SOCKET.bind(SERVER_ADDRESS)
SERVER_SOCKET.listen()

# start connection
CLIENT_SOCKET, CLIENT_ADDRESS = SERVER_SOCKET.accept()
if CLIENT_SOCKET:
    print(f'Connection Established!\nClient Address: {CLIENT_ADDRESS}\nTime: {datetime.datetime.now()}')

# get values /w record
received_messages_list = []

while CLIENT_SOCKET:

    client_bytes = CLIENT_SOCKET.recv(1024)

    if client_bytes == b'':
        print('Client disconnected.')
        break

    bytes_received_w_record = {
        'agent_address' : CLIENT_ADDRESS,
        'time' : datetime.datetime.now(),
        'client_message' : client_bytes.decode()
                             }


    print(bytes_received_w_record)

    received_messages_list.append(bytes_received_w_record)

CLIENT_SOCKET.close()
SERVER_SOCKET.close()
