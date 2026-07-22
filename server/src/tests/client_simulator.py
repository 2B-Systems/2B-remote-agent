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

def message_delay_choice() -> bool:

    while True:
        choice = input(
            "Do you want a delay between messages? (Y/n): "
        ).strip().lower()

        if choice in ("", "y", "yes"):
            return True

        if choice in ("n", "no"):
            return False

        print("Please enter Y or N.")

async def message_delay():
        
        seconds_to_wait = random.randint(1, 5)
        print(f'Waiting {seconds_to_wait} seconds..')
        await asyncio.sleep(seconds_to_wait)

async def tester_agent(message_list: list = TELEMETRY_POOL):

    reader, writer = await asyncio.open_connection(SERVER_IP, SERVER_PORT)
    use_delay = message_delay_choice()

    try:
        while True:

            message = random.choice(message_list)

            print(f'Sending message: {message}')
            writer.write(message.encode())
            await writer.drain()

            if use_delay:
                await message_delay()

    finally:
        writer.close()
        await writer.wait_closed()
        
if __name__ == "__main__":
    try:
        asyncio.run(tester_agent())
    except KeyboardInterrupt:
        print("\nSimulator stopped.")
