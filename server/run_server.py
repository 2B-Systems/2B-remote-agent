import asyncio
import pathlib

from src.server_config import ServerConfig
from src.server import Server

DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT = "127.0.0.1", 8080
CURRENT_FOLDER_PATH = pathlib.Path(__file__).resolve().parent
LOGS_FOLDER_PATH = CURRENT_FOLDER_PATH / "logs"


def get_ip() -> str:
    desired_value = input(
        "Please insert the desired IP Address or press ENTER for the default value: "
    )

    if desired_value == "":
        return DEFAULT_SERVER_IP

    return desired_value


def get_port() -> int:
    while True:
        desired_value = input(
            "Please insert the desired PORT value or press ENTER for the default value: "
        )

        if desired_value == "":
            return DEFAULT_SERVER_PORT

        try:
            desired_value = int(desired_value)
            return desired_value

        except ValueError:
            print("The inserted PORT value is illegal.")


def configure_server() -> ServerConfig:
    while True:
        try:
            inserted_ip = get_ip()
            inserted_port = get_port()

            # server config
            config = ServerConfig(inserted_ip, inserted_port)

            # validate config
            if not config.is_server_ip_valid():
                raise ValueError(f"Invalid server IP: {config.server_ip}")

            if not config.is_server_port_valid():
                raise ValueError(f"Invalid server port: {config.server_port}")

            else:
                return config

        except ValueError as error:
            print(error)


async def run_server(config: ServerConfig) -> None:
    # server
    server = Server(config.server_ip, config.server_port, LOGS_FOLDER_PATH)
    try:
        print(f"Starting server on {config.server_ip}:{config.server_port}...")
        await server.initialize()
        await server.serve()

    finally:
        print("\nServer is shutting down.")
        # close server
        await server.close()


if __name__ == "__main__":
    server_config = configure_server()
    asyncio.run(run_server(server_config))
