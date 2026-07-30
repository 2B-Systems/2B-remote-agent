import asyncio
import datetime
import pathlib
import aiofiles


# client coroutine
async def handle_client(reader: asyncio.StreamReader, writer: asyncio.StreamWriter):

    # get the client address
    CLIENT_ADDRESS = writer.get_extra_info("peername")
    CLIENT_IP, CLIENT_PORT = CLIENT_ADDRESS

    print(f"Client successfully connected.\nClient Address: {CLIENT_ADDRESS}")

    # log folder operations
    current_folder = pathlib.Path(__file__).resolve().parent
    logs_folder = current_folder.parent / "logs"

    logs_folder.mkdir(parents=True, exist_ok=True)

    print('Creating "logs" folder, skipping if it exists.')

    client_identifier = client_identifier = f"{CLIENT_IP}_{CLIENT_PORT}"

    # get values /w record
    while True:

        client_bytes = await reader.read(1024)

        if not client_bytes:
            print("Client disconnected.")
            break

        bytes_received_w_record = {
            "time": str(datetime.datetime.now())[0:-7],
            "client_message": client_bytes.decode(),
        }

        current_byte_time, current_byte_message = (
            bytes_received_w_record["time"],
            bytes_received_w_record["client_message"],
        )

        # save to file
        information_to_save_to_file = f"[{bytes_received_w_record['time']}] Client Message: {bytes_received_w_record['client_message']}"

        if current_byte_message.lower().startswith("username:"):
            client_username = current_byte_message.split(":", 1)[1].strip()

            if client_username:
                client_identifier = client_username

        log_path = logs_folder / f"{client_identifier}.log"

        async with aiofiles.open(log_path, "a", encoding="utf-8") as log_file:
            await log_file.write(information_to_save_to_file)

        print(information_to_save_to_file)

    # close connection
    writer.close()
    await writer.wait_closed()


async def main(server_ip: str, server_port: int) -> None:
    # server init
    server = await asyncio.start_server(handle_client, server_ip, server_port)

    print(f"Server is listening to {server_ip}:{server_port}")

    await server.serve_forever()

    server.close()
    await server.wait_closed()
