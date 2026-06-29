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

while CLIENT_SOCKET:

    CLIENT_BYTES = CLIENT_SOCKET.recv(1024)

    BYTES_LIST = []

    BYTES_RECEIVED_W_TIME = {
        'agent_address' : CLIENT_ADDRESS,
        'time' : datetime.datetime.now(),
        'bytes_data': CLIENT_BYTES,
        'client_message' : CLIENT_BYTES.decode()
                             }

    BYTES_LIST.append(BYTES_RECEIVED_W_TIME)
