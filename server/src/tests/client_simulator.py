import asyncio
import random

TELEMETRY_POOL = [
    "event=heartbeat status=alive uptime=1432s\n",
    "event=cpu_usage value=17.4 unit=percent cores=8\n",
    "event=memory_usage used=2841 total=7936 unit=MB\n",
    "event=disk_usage mount=/ used=41.7 unit=percent\n",
    "event=network_rx interface=eth0 bytes=184320 packets=241\n",
    "event=network_tx interface=eth0 bytes=96211 packets=137\n",
    "event=connection status=established remote=192.168.1.42:51824\n",
    "event=process_started pid=1842 name=telemetry-agent\n",
    "event=process_stopped pid=1842 exit_code=0\n",
    "event=service_status name=ssh status=running\n",
    "event=temperature sensor=cpu value=54.2 unit=celsius\n",
    "event=battery level=76 unit=percent charging=false\n",
    "event=user_login username=test-user method=ssh result=success\n",
    "event=user_login username=unknown method=ssh result=failed\n",
    "event=file_created path=/tmp/agent-test.txt size=128\n",
    "event=file_modified path=/var/log/system.log size=4096\n",
    "event=warning component=memory message=\"usage above threshold\"\n",
    "event=error component=network code=ECONNRESET message=\"connection reset by peer\"\n",
    "event=update status=available version=1.4.2\n",
    "event=unicode_test message=\"Türkçe karakterler: ğüşçıİöç\"\n",

    # edge cases

    "\n",
    "event=empty_value value=\n",
    "event=large_number value=999999999999999999999\n",
    "event=unknown type=totally_unrecognized\n",
    "event=unicode emoji=🚀 status=çalışıyor\n",
    "malformed telemetry without key value formatting\n",
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

async def tester_agent(server_ip: str, server_port: int,message_list: list = TELEMETRY_POOL) -> None:

    reader, writer = await asyncio.open_connection(server_ip, server_port)
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