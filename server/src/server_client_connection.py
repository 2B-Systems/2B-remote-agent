# standard-library modules
import asyncio
import datetime as dt
import pathlib

# third-party modules
import aiofiles


class ServerClientConnection:
    def __init__(
        self,
        reader: asyncio.StreamReader,
        writer: asyncio.StreamWriter,
    ) -> None:
        self.reader = reader
        self.writer = writer

    def get_client_address(self) -> str:
        peername = self.writer.get_extra_info("peername")
        client_ip, client_port = peername[0], peername[1]

        # replace path separators and IPv6 colons to create a safe log filename
        safe_ip = str(client_ip).replace(":", "_").replace("/", "_").replace("\\", "_")

        self.client_address = f"{client_ip}:{client_port}"
        self.client_identifier = f"{safe_ip}_{client_port}"

        return self.client_address

    async def receive_record(self, include_time: bool = True) -> str | None:
        # each newline marks the end of one protocol record
        client_bytes = await self.reader.readline()
        if not client_bytes:
            print("Client disconnected.")
            return
        current_time = dt.datetime.now().isoformat(timespec="seconds")
        current_data = client_bytes.decode()
        if include_time:
            current_record = f"[{current_time}] {current_data}"
        else:
            current_record = current_data

        return current_record

    async def save_records_to_file(
        self, logs_folder: pathlib.Path, record: str
    ) -> None:
        client_log_file_path = logs_folder / f"{self.client_identifier}.log"
        # write the current record to client's log file
        async with aiofiles.open(
            client_log_file_path, "a", encoding="utf-8"
        ) as client_log_file:
            await client_log_file.write(record)

    async def close(self) -> None:
        self.writer.close()
        await self.writer.wait_closed()
