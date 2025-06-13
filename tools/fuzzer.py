#!/usr/bin/env python3

import hashlib
from io import StringIO
import os
import pathlib
import random
import subprocess
from typing import IO

SCRIPT_PATH = pathlib.Path(__file__)
ROOT_DIR = SCRIPT_PATH.parent.parent

class TypeInfo:
    def __init__(self, name: str):
        self.name = name

TYPE_BOOL = TypeInfo('bool')
TYPE_I32 = TypeInfo('i32')

class FunctionInfo:
    def __init__(self, name: str, return_type: TypeInfo):
        self.name = name
        self.return_type = return_type

class Context:
    def __init__(self):
        self.functions: list[FunctionInfo] = []

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

def write_bool_literal(io: IO[str]) -> None:
    if random.randint(0, 1) == 0:
        value = 'false'
    else:
        value = 'true'
    io.write(value)

def write_bool_binary_expression(io: IO[str]) -> None:
    write_bool_expression(io)

    op = random.choice(['&', '|'])
    io.write(f' {op} ')

    write_bool_expression(io)

def write_bool_expression(io: IO[str]) -> None:
    r = random.randint(0, 2)
    if r == 0:
        write_bool_binary_expression(io)
    else:
        write_bool_literal(io)

def write_int_literal(io: IO[str]) -> None:
    r = random.randrange(0, 2)
    if r == 0:
        i = random.choice([
            '0b0',
            '0o0',
            '0',
            '123',
            '0x0',
            # '0xfeedface',
        ])
    else:
        i = str(random.randint(-1_000_000, 1_000_000))

    io.write(i)

def write_int_binary_expression(io: IO[str]) -> None:
    write_int_expression(io)

    op = random.choice(['+', '-', '*', '&', '|', '^'])
    io.write(f' {op} ')

    write_int_expression(io)

def write_int_expression(io: IO[str]) -> None:
    if random.randint(0, 2) == 0:
        write_int_binary_expression(io)
    else:
        write_int_literal(io)

def write_expression(io: IO[str], type: TypeInfo) -> None:
    if type.name == 'bool':
        write_bool_expression(io)
    elif type.name == 'i32':
        write_int_expression(io)
    else:
        assert False, f"Unexpected type '{type.name}'"

def write_identifier(io: IO[str]) -> None:
    io.write(random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_'))
    for _ in range(random.randint(0, 14)):
        io.write(random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789'))

def write_function(io: IO[str], context: Context, function: FunctionInfo) -> None:
    io.write('fun ')
    io.write(function.name)
    io.write('() ')
    io.write(function.return_type.name)
    io.write('\n{\n    return ')
    write_expression(io, function.return_type)
    io.write(';\n}\n')

def write_code(io: IO[str]) -> None:
    context = Context()
    for _ in range(random.randint(2, 10)):
        name = StringIO()
        write_identifier(name)
        type = random.choice([TYPE_BOOL, TYPE_I32])
        function = FunctionInfo(name.getvalue(), type)
        context.functions.append(function)

    for function in context.functions:
        write_function(io, context, function)

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
