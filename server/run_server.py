import asyncio

from src.config_setup import config_setup
from src.server import main as server_main


def run() -> None:
    server_ip, server_port = config_setup()

    try:
        asyncio.run(server_main(server_ip, server_port))
    except KeyboardInterrupt:
        print("\nServer is shutting down.")


if __name__ == "__main__":
    run()