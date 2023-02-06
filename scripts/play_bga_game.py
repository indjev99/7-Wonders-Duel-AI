from pipe_reader_writer import PipeReaderWriter

def main():
    pipe = PipeReaderWriter('//./pipe/7wdai')

    while True:
        print(pipe.read())

if __name__ == '__main__':
    main()
