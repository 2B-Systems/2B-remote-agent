import time
from src.config import SERVER_IP, SERVER_PORT
import asyncio
import random

TELEMETRY_POOL = [

    'WAZZZZZZZZZZZZZZZZZZZZZZZZZAAAAAAAAAAAAAAPPPPPPPPPPPP',
    "hey, I'm a simulator bruv",
    'YEEEEEEEEEEEEEEEEHAWWWWWWWWW',
    'HELLLLLLLL YEAAAAAAHHHHHHHHHHH',
    '67',
    'hocam durun binary kodunuzu not alıyorum..'

]

async def tester_agent(message_list: list = TELEMETRY_POOL):
    reader, writer = await asyncio.open_connection(SERVER_IP, SERVER_PORT)

    while True:

        message = random.choice(message_list)

        print(f'Sending message: {message}')
        writer.write(message.encode())
        await writer.drain()

        # wait
        seconds_to_wait = random.randint(1, 5)
        print(f'Waiting {seconds_to_wait} seconds..')
        time.sleep(seconds_to_wait)

if __name__ == "__main__":
    try:
        asyncio.run(tester_agent())
    except KeyboardInterrupt:
        print("\nSimulator stopped.")
