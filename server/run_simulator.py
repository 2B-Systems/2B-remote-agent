# standard-library modules
import asyncio

# classes from server/src
from src.tests.client_simulator import ClientSimulator
from src.server_config import ServerConfig

# hardcoded default values for IP and PORT
DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT = "127.0.0.1", 8080

# hardcoded TELEMETRY_POOL list containing a bunch of 'str'
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
    'event=warning component=memory message="usage above threshold"\n',
    'event=error component=network code=ECONNRESET message="connection reset by peer"\n',
    "event=update status=available version=1.4.2\n",
    'event=unicode_test message="Türkçe karakterler: ğüşçıİöç"\n',
    # edge cases
    "",
    "event=empty_value value=\n",
    "event=large_number value=999999999999999999999\n",
    "event=unknown type=totally_unrecognized\n",
    "event=unicode emoji=🚀 status=çalışıyor\n",
    "malformed telemetry without key value formatting\n",
]

# getting IP from user
def get_ip() -> str:
    desired_value = input(
        "Please insert the desired IP Address to connect or press ENTER for the default value:\n"
    )

    if desired_value == "":
        return DEFAULT_SERVER_IP

    return desired_value

# getting PORT from user
def get_port() -> int:
    while True:
        desired_value = input(
            "Please insert the desired PORT value or press ENTER for the default value:\n"
        )

        if desired_value == "":
            return DEFAULT_SERVER_PORT

        try:
            desired_value = int(desired_value)
            return desired_value

        except ValueError:
            print("The inserted PORT value is illegal.\n")

# getting input from the user
def configure_simulator() -> ServerConfig:
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


def get_message_delay_choice() -> bool:
    while True:
        choice = input("Do you want a delay between messages? (Y/n):\n").strip().lower()
        if choice in ("", "y", "yes"):
            return True
        if choice in ("n", "no"):
            return False
        print("Please enter Y or N.")


async def run_simulator(
    telemetry_pool: list = TELEMETRY_POOL,
    server_ip: str = DEFAULT_SERVER_IP,
    server_port: int = DEFAULT_SERVER_PORT,
    message_delay: bool = True,
) -> None:
    client_simulator = ClientSimulator(telemetry_pool, message_delay)
    await client_simulator.open_connection(server_ip, server_port)

    try:
        while True:
            await client_simulator.send_message()

    finally:
        print("Agent is shutting down.\n")
        await client_simulator.close_agent()


if __name__ == "__main__":
    config = configure_simulator()
    message_delay = get_message_delay_choice()
    asyncio.run(
        run_simulator(
            TELEMETRY_POOL, config.server_ip, config.server_port, message_delay
        )
    )
