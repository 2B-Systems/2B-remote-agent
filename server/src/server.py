import asyncio
import datetime
import pathlib

# set the server address
SERVER_IP, SERVER_PORT = '127.0.0.1', 8080
SERVER_ADDRESS = (SERVER_IP, SERVER_PORT)

# client coroutine
async def handle_client(reader: asyncio.StreamReader, writer: asyncio.StreamWriter):

    # get the client address
    CLIENT_ADDRESS = writer.get_extra_info('peername')
    CLIENT_IP, CLIENT_PORT = CLIENT_ADDRESS

    print(f'Client successfully connected.\nClient Address: {CLIENT_ADDRESS}')

    # log folder operations
    # FIXME handle exceptions for folder creations
    folder_for_client = pathlib.Path(f'server/logs/{CLIENT_IP}_{CLIENT_PORT}')

    if not folder_for_client.exists():
        folder_for_client.mkdir()
    else:
        print('Log directory exists, skipping folder creations...')

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

        # save to JSON
        information_to_save_to_json = f'[{current_byte_time}] Client Message: {current_byte_message}'

        with open(f'{folder_for_client}/events.log', 'a') as log_file:
            log_file.write(information_to_save_to_json)

        print(information_to_save_to_json)

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

asyncio.run(main())
