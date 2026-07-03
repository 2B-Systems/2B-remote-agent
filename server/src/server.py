
from src.config import SERVER_IP, SERVER_PORT
import asyncio
import datetime
import pathlib
import aiofiles

# client coroutine
async def handle_client(reader: asyncio.StreamReader, writer: asyncio.StreamWriter):

    # get the client address
    CLIENT_ADDRESS = writer.get_extra_info('peername')
    CLIENT_IP, CLIENT_PORT = CLIENT_ADDRESS

    print(f'Client successfully connected.\nClient Address: {CLIENT_ADDRESS}')

    # log folder operations
    folder_for_client = pathlib.Path(__file__).parent
    
    folder_for_client.mkdir(parents = True, exist_ok = True)

    # get values /w record
    while True:

        client_bytes = await reader.read(1024)

        if not client_bytes:
            print('Client disconnected.')
            break

        bytes_received_w_record = {
            'time' : datetime.datetime.now(),
            'client_message' : client_bytes.decode()
                                 }

        current_byte_time, current_byte_message = bytes_received_w_record['time'],bytes_received_w_record['client_message']

        # save to file
        information_to_save_to_file = f'[{current_byte_time}] Client Message: {current_byte_message}\n'

        async with aiofiles.open(f'{folder_for_client}/events.log', 'a') as log_file:
            await log_file.write(information_to_save_to_file)

        print(information_to_save_to_file)

    # close connection
    writer.close()
    await writer.wait_closed()


async def main():
    # server init
    server = await asyncio.start_server(handle_client, SERVER_IP, SERVER_PORT)

    print(f'Server is listening to {SERVER_IP}:{SERVER_PORT}')

    await server.serve_forever()

    server.close()
    await server.wait_closed()

if __name__ == "__main__":
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\nSimulator stopped.")
