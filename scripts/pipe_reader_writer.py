import time
import win32pipe
import win32file
import pywintypes

class PipeReaderWriter():
    def __init__(self, pipe_name: str):
        self.pipe_handle = None
        while self.pipe_handle is None:
            try:
                self.pipe_handle = win32file.CreateFile(
                    pipe_name,
                    win32file.GENERIC_READ | win32file.GENERIC_WRITE,
                    0,
                    None,
                    win32file.OPEN_EXISTING,
                    0,
                    None
                )
            except pywintypes.error as e:
                if e.args[0] == 2:
                    time.sleep(1)
                else:
                    print('Unexpected error:', e.args[0])
                    exit(-1)

        win32pipe.SetNamedPipeHandleState(self.pipe_handle, win32pipe.PIPE_READMODE_MESSAGE, None, None)

    def write(self, s: str):
        s_bytes = str.encode(s)
        code, bytes_written = win32file.WriteFile(self.pipe_handle, s_bytes)

        if code != 0 or bytes_written != len(s):
            print('Failed WriteFile:')
            exit(-1)

    def read(self) -> str:
        code, s_bytes = win32file.ReadFile(self.pipe_handle, PipeReaderWriter.BUF_SIZE)

        if code != 0:
            print('Bad code from ReadFile:', code)
            exit(-1)

        return s_bytes.decode()

    BUF_SIZE = 4 * 1024
