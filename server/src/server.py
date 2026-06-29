import socket
import datetime

SERVER_IP = '127.0.0.1'
SERVER_PORT = 8080
SERVER_ADDRESS = (SERVER_IP, SERVER_PORT)

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.bind(SERVER_ADDRESS)
sock.listen()

CLIENT_SOCKET, CLIENT_ADDRESS = sock.accept()
if CLIENT_SOCKET:
    print(f'Connection Established!\nClient Address: {CLIENT_ADDRESS}\nTime: {datetime.datetime.now()}')

BYTES_LIST = []

while CLIENT_SOCKET:

    CLIENT_BYTES = CLIENT_SOCKET.recv(1024)

    if CLIENT_BYTES == b'':
        print('Client disconnected.')
        break

    BYTES_RECEIVED_W_METADATA = {
        'agent_address' : CLIENT_ADDRESS,
        'time' : datetime.datetime.now(),
        'bytes_data': CLIENT_BYTES,
        'client_message' : CLIENT_BYTES.decode()
                             }


    print(BYTES_RECEIVED_W_METADATA)

    BYTES_LIST.append(BYTES_RECEIVED_W_METADATA)

CLIENT_SOCKET.close()
sock.close()
