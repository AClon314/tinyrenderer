#!/bin/env python
import os
import time


def get_mtime(filepath):
    return os.path.getmtime(filepath)


def main():
    filepath = 'main.cpp'
    script = 'build/g'
    last_mtime = get_mtime(filepath)

    while True:
        time.sleep(1)
        current_mtime = get_mtime(filepath)
        if current_mtime != last_mtime:
            os.system('clear')
            os.system(script)
            last_mtime = current_mtime


if __name__ == "__main__":
    main()
