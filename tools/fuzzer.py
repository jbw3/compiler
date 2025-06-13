#!/usr/bin/env python3

import hashlib
import os
import pathlib
import random
import subprocess
from typing import IO, Iterator

SCRIPT_PATH = pathlib.Path(__file__)
ROOT_DIR = SCRIPT_PATH.parent.parent

def md5(filename: pathlib.Path) -> str:
    m = hashlib.md5()
    with open(filename, 'rb') as f:
        buff = f.read(4096)
        while buff != b'':
            m.update(buff)
            buff = f.read(4096)

    return m.hexdigest()

def run_compiler(src_filename: pathlib.Path) -> int:
    compiler_path = ROOT_DIR / 'debug' / 'compiler' / 'wip'
    cmd = [
        compiler_path,
        src_filename,
    ]
    proc = subprocess.run(cmd)
    return proc.returncode

def write_int(io: IO[str]) -> None:
    r = random.randrange(0, 2)
    if r == 0:
        i = random.choice([
            '0b0',
            '0o0',
            '0',
            '123',
            '0x0',
            '0xfeedface',
        ])
    else:
        i = str(random.randint(-1_000_000, 1_000_000))

    io.write(i)

def write_int_expression(io: IO[str]) -> None:
    if random.randint(0, 1) == 0:
        write_int_expression(io)
    else:
        write_int(io)

    op = random.choice(['+', '-', '*', '/', '%'])
    io.write(f' {op} ')

    if random.randint(0, 1) == 0:
        write_int_expression(io)
    else:
        write_int(io)

def write_identifier(io: IO[str]) -> None:
    io.write(random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_'))
    for i in range(random.randint(0, 14)):
        io.write(random.choice('abcABC0123456789_'))

def write_function(io: IO[str]) -> None:
    io.write('fun ')
    write_identifier(io)
    io.write('() i32\n{\n    return ')
    write_int_expression(io)
    io.write(';\n}\n')

def write_code(io: IO[str]) -> None:
    for _ in range(random.randint(1, 3)):
        write_function(io)

def main() -> None:
    src_filename = pathlib.Path('fuzzer.wip')

    error_count = 0
    for _ in range(10):
        with open(src_filename, 'w') as f:
            write_code(f)

        rc = run_compiler(src_filename)
        if rc != 0:
            error_count += 1
            print(f'Error: rc={rc}')
            h = md5(src_filename)
            src_filename.rename(f'fuzzer-{h}.wip')

    if src_filename.exists():
        os.remove(src_filename)

if __name__ == '__main__':
    main()
