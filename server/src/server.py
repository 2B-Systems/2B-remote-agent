import asyncio
import pathlib
from src.server_client_connection import ServerClientConnection


class Server:

    def __init__(
        self, server_ip: str, server_port: int, logs_folder: pathlib.Path
    ) -> None:
        # server initialization
        self.server_ip = server_ip
        self.server_port = server_port
        self.logs_folder = logs_folder
        self.server: asyncio.AbstractServer | None = None

    async def handle_client(
        self,
        reader: asyncio.StreamReader,
        writer: asyncio.StreamWriter,
    ) -> None:
        client = ServerClientConnection(reader, writer)

        try:
            client.get_client_address()

            # receive and save newline-delimited records until the client disconnects
            while True:
                returned_record = await client.receive_record()
                if returned_record is None:
                    break
                await client.save_records_to_file(self.logs_folder, returned_record)
                print(returned_record)

        finally:
            await client.close()

    async def initialize(self) -> None:
        # create the runtime logs folder before accepting client connections
        self.logs_folder.mkdir(parents=True, exist_ok=True)
        self.server = await asyncio.start_server(
            self.handle_client, self.server_ip, self.server_port
        )

    async def serve(self) -> None:
        if self.server is None:
            raise RuntimeError("Server must be initialized before serving.\n")

        await self.server.serve_forever()

    async def close(self) -> None:
        if self.server is None:
            return

        self.server.close()
        await self.server.wait_closed()
