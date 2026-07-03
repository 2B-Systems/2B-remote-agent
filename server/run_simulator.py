import asyncio
from src.tests.client_simulator import tester_agent

if __name__ == "__main__":
    try:
        asyncio.run(tester_agent())
    except KeyboardInterrupt:
        print("\nSimulator stopped.")