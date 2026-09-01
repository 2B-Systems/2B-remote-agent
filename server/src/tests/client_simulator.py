import asyncio
import random


class ClientSimulator:

    def __init__(self, telemetry_pool: list, message_delay: bool = True) -> None:
        self.telemetry_pool = telemetry_pool
        self.message_delay = message_delay
        self.username_sent = False

    async def delay_message(self, max_delay_time: int = 5) -> None:
        seconds_to_wait = random.randint(1, max_delay_time)
        print(f"Waiting {seconds_to_wait} seconds..")
        await asyncio.sleep(seconds_to_wait)

    async def open_connection(self, server_ip: str, server_port: int):
        self.reader, self.writer = await asyncio.open_connection(server_ip, server_port)

    async def send_message(self) -> None:
        use_delay = self.message_delay

        message = random.choice(self.telemetry_pool)

        # identify the simulator once before sending telemetry on the same connection
        if not self.username_sent:
            message = "Username: SIMULATOR\n"
            self.username_sent = True

        # make every outgoing message one newline-delimited record
        message = message.rstrip("\n") + "\n"
        print(f"Sending message: {message}")
        self.writer.write(message.encode())
        await self.writer.drain()
        if use_delay:
            await self.delay_message()

    async def close_agent(self):
        self.writer.close()
        await self.writer.wait_closed()
