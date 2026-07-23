import ipaddress
from .default_config import DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT
INFORMATION_MESSAGE = "You'll be prompted to enter values for the SERVER ADDRESS.\n" + f"If you don't know which value to set, press ENTER for the default values\n[DEFAULT_IP: {DEFAULT_SERVER_IP}, DEFAULT_PORT = {DEFAULT_SERVER_PORT}]"

def port_is_valid(port: int) -> bool:
    if 1 <= port <= 65535:
         return True
    else:
         return False

def config_setup():

    print(INFORMATION_MESSAGE)

    while True:
        try:
            server_ip = input("Please enter SERVER IP: ")

            match server_ip:
                case '':
                    server_ip = ipaddress.ip_address(DEFAULT_SERVER_IP)
                    server_ip = str(server_ip)
                    print("Default value set")
                    break

                case _:
                    server_ip = ipaddress.ip_address(server_ip)
                    server_ip = str(server_ip)
                    print("Default value set")
                    break

        except ValueError:
            print("The server_ip does not comply with the required IP addressing rules.")

    while True:
        try:
            server_port = input("Please enter SERVER PORT: ")             
                
            if server_port == '':
                server_port = DEFAULT_SERVER_PORT

            else:
                server_port = int(server_port)

            if port_is_valid(server_port):
                break

            else:
                raise ValueError
 
            
        except ValueError:
            print("The server_port does not comply with the required rules.")

    print(f'Values successfully set.\nSERVER_IP: {server_ip}, SERVER_PORT: {server_port}')
    return server_ip, server_port

if __name__ == "__main__":
    try:
        config_setup()
    except KeyboardInterrupt:
        print("Setup aborted.")