import pywintypes
import sys
import time
import win32pipe
import win32file

class PipeReaderWriter():
    def __init__(self, pipe_name: str):
        self.pipe_handle = None
        while self.pipe_handle is None:
            try:
                print('Trying to open pipe', file=sys.stderr)
                self.pipe_handle = win32file.CreateFile(
                    pipe_name,
                    win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                    0,
                    None,
                    win32file.OPEN_EXISTING,
                    0,
                    None
                )
                print('Opened pipe', file=sys.stderr)
            except pywintypes.error as e:
                if e.args[0] == 2:
                    time.sleep(1)
                else:
                    print('Unexpected error:', e.args[0], file=sys.stderr)
                    exit(-1)

        win32pipe.SetNamedPipeHandleState(self.pipe_handle, win32pipe.PIPE_READMODE_MESSAGE | win32pipe.PIPE_NOWAIT, None, None)

    def write(self, s: str) -> None:
        s_bytes = str.encode(s)
        code, bytes_written = win32file.WriteFile(self.pipe_handle, s_bytes)

        print(f'< {s}', file=sys.stderr)

        if code != 0 or bytes_written != len(s):
            print('Failed WriteFile.', file=sys.stderr)
            exit(-1)

    def read(self) -> str:
        try:
            _, s_bytes = win32file.ReadFile(self.pipe_handle, PipeReaderWriter.BUF_SIZE)
            s = s_bytes.decode()

            if s != '':
                print(f'> {s}', file=sys.stderr)

            return s
        except Exception:
            return ''

    BUF_SIZE = 4 * 1024
