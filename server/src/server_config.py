import ipaddress


class ServerConfig:

    def __init__(self, server_ip: str = "127.0.0.1", server_port: int = 8080) -> None:
        self.server_ip = server_ip
        self.server_port = server_port

    def is_server_port_valid(self) -> bool:
        return 1 <= self.server_port <= 65535

    def is_server_ip_valid(self) -> bool:
        try:
            ipaddress.ip_address(self.server_ip)
            return True
        except ValueError:
            return False
