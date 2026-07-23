import asyncio
from src.config_setup import config_setup
from src.tests.client_simulator import tester_agent

def run():
    try:
        server_ip, server_port = config_setup()
        asyncio.run(tester_agent(server_ip, server_port))

    except KeyboardInterrupt:
        print("Agent is shutting down.")

if __name__ == "__main__":
        run()
    