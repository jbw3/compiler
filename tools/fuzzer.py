#!/usr/bin/env python3

import argparse
import hashlib
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

TYPE_FUN = TypeInfo('fun')
TYPE_BOOL = TypeInfo('bool')
TYPE_I32 = TypeInfo('i32')
TYPE_STR = TypeInfo('str')

EXPRESSION_TYPES = [
    TYPE_BOOL,
    TYPE_I32,
    TYPE_STR,
]

INVALID_IDENTIFIERS = {
    'bool',
    'cast',
    'const',
    'elif',
    'else',
    'for',
    'fun',
    'i8',
    'i16',
    'i32',
    'i64',
    'if',
    'in',
    'isize',
    'str',
    'type',
    'u8',
    'u16',
    'u32',
    'u64',
    'usize',
    'var',
    'while',
}

class ParamInfo:
    def __init__(self, name: str, type: TypeInfo):
        self.name = name
        self.type = type

class FunctionInfo:
    def __init__(self, name: str, params: list[ParamInfo], return_type: TypeInfo):
        self.name = name
        self.params = params
        self.return_type = return_type

class IdentifierInfo:
    def __init__(self, name: str, type: TypeInfo):
        self.name = name
        self.type = type

class Context:
    def __init__(self):
        self.functions: list[FunctionInfo] = []
        self.identifier_stack: list[list[IdentifierInfo]] = [[]]
        self.indent_level = 0

    def add_identifier(self, identifier: IdentifierInfo) -> None:
        self.identifier_stack[-1].append(identifier)

    def clear_current_identifier_scope(self) -> None:
        self.identifier_stack[-1].clear()

    def get_current_identifier_scope(self) -> list[IdentifierInfo]:
        return self.identifier_stack[-1]

    def push_scope(self) -> None:
        self.identifier_stack.append(self.identifier_stack[-1][:])

    def pop_scope(self) -> None:
        self.identifier_stack.pop()

    def get_identifier_of_type(self, type: TypeInfo) -> IdentifierInfo|None:
        ids = [i for i in self.identifier_stack[-1] if i.type.name == type.name]
        if len(ids) == 0:
            return None
        return random.choice(ids)

    def get_function_with_return_type(self, return_type: TypeInfo|None=None) -> FunctionInfo|None:
        if return_type is None:
            functions = self.functions
        else:
            functions = [f for f in self.functions if f.return_type.name == return_type.name]

        if len(functions) == 0:
            return None
        return random.choice(functions)

def md5(filename: pathlib.Path) -> str:
    m = hashlib.md5()
    with open(filename, 'rb') as f:
        buff = f.read(4096)
        while buff != b'':
            m.update(buff)
            buff = f.read(4096)

    return m.hexdigest()

def run_compiler(src_filename: pathlib.Path, out_filename: pathlib.Path) -> int:
    compiler_path = ROOT_DIR / 'debug' / 'compiler' / 'wip'
    cmd = [
        compiler_path,
        src_filename,
        '-o',
        out_filename,
    ]
    proc = subprocess.run(cmd)
    return proc.returncode

def get_indent_str(context: Context) -> str:
    return ' ' * (context.indent_level * 4)

def get_identifier(context: Context) -> str:
    invalid: set[str] = {i.name for i in context.get_current_identifier_scope()}
    invalid |= {f.name for f in context.functions}
    invalid |= INVALID_IDENTIFIERS

    valid = False
    while not valid:
        identifier = random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_')
        for _ in range(random.randint(0, 14)):
            identifier += random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789')

        valid = identifier not in invalid

    return identifier

def write_identifier_expression(io: IO[str], context: Context, type: TypeInfo) -> bool:
    identifier = context.get_identifier_of_type(type)
    if identifier is None:
        return False

    io.write(identifier.name)
    return True

def write_function_call_expression(io: IO[str], context: Context, return_type: TypeInfo|None) -> bool:
    function = context.get_function_with_return_type(return_type)
    if function is None:
        return False

    io.write(function.name)
    io.write('(')
    for param in function.params:
        write_expression(io, context, param.type)
        io.write(', ')
    io.write(')')
    return True

def write_bool_literal(io: IO[str]) -> None:
    if random.randint(0, 1) == 0:
        value = 'false'
    else:
        value = 'true'
    io.write(value)

def write_bool_binary_expression(io: IO[str], context: Context) -> None:
    write_bool_expression(io, context)

    op = random.choice(['&', '|'])
    io.write(f' {op} ')

    write_bool_expression(io, context)

def write_bool_expression(io: IO[str], context: Context) -> None:
    r = random.randint(0, 9)
    if 0 <= r <= 1:
        write_bool_binary_expression(io, context)
    elif 2 <= r <= 4:
        ok = write_identifier_expression(io, context, TYPE_BOOL)
        if not ok:
            write_bool_literal(io)
    elif 5 <= r <= 5:
        ok = write_function_call_expression(io, context, TYPE_BOOL)
        if not ok:
            write_bool_literal(io)
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

def write_int_binary_expression(io: IO[str], context: Context) -> None:
    write_int_expression(io, context)

    op = random.choice(['+', '-', '*', '&', '|', '^'])
    io.write(f' {op} ')

    write_int_expression(io, context)

def write_int_expression(io: IO[str], context: Context) -> None:
    r = random.randint(0, 9)
    if 0 <= r <= 1:
        write_int_binary_expression(io, context)
    elif 2 <= r <= 4:
        ok = write_identifier_expression(io, context, TYPE_I32)
        if not ok:
            write_int_literal(io)
    elif 5 <= r <= 5:
        ok = write_function_call_expression(io, context, TYPE_I32)
        if not ok:
            write_int_literal(io)
    else:
        write_int_literal(io)

REALLY_LONG_STRING = 'This is a ' + ('really, ' * 20) + 'long string.'

def write_str_literal(io: IO[str]) -> None:
    io.write('"')

    r = random.randint(0, 4)
    if r == 0:
        io.write(random.choice([
            '',
            ' ',
            '\t',
            'café',
            REALLY_LONG_STRING,
        ] + list(INVALID_IDENTIFIERS)))
    else:
        for _ in range(random.randint(1, 12)):
            io.write(random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()-=_+πé'))

    io.write('"')

def write_str_expression(io: IO[str], context: Context) -> None:
    r = random.randint(0, 9)
    if 0 <= r <= 3:
        ok = write_identifier_expression(io, context, TYPE_STR)
        if not ok:
            write_str_literal(io)
    elif 4 <= r <= 4:
        ok = write_function_call_expression(io, context, TYPE_STR)
        if not ok:
            write_str_literal(io)
    else:
        write_str_literal(io)

def write_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    if type.name == 'bool':
        write_bool_expression(io, context)
    elif type.name == 'i32':
        write_int_expression(io, context)
    elif type.name == 'str':
        write_str_expression(io, context)
    else:
        assert False, f"Unexpected type '{type.name}'"

def write_variable_declaration(io: IO[str], context: Context) -> None:
    name = get_identifier(context)
    type = random.choice(EXPRESSION_TYPES)

    io.write(get_indent_str(context))
    io.write('var ')
    io.write(name)
    io.write(' ')
    io.write(type.name)
    io.write(' = ')
    write_expression(io, context, type)
    io.write(';\n')

    context.add_identifier(IdentifierInfo(name, type))

def write_if_statement(io: IO[str], context: Context) -> None:
    io.write(get_indent_str(context))
    io.write('if ')
    write_bool_expression(io, context)
    io.write('\n')
    io.write(get_indent_str(context))
    io.write('{\n')
    context.indent_level += 1

    context.push_scope()
    for _ in range(random.randint(1, 5)): # TODO: use range 0-5 when compiler bug is fixed
        write_statement(io, context)
    context.pop_scope()

    context.indent_level -= 1
    io.write(get_indent_str(context))
    io.write('}\n')

def write_statement(io: IO[str], context: Context) -> None:
    r = random.randint(0, 6)
    if r == 0:
        write_if_statement(io, context)
    elif 1 <= r <= 3:
        write_variable_declaration(io, context)
    else:
        io.write(get_indent_str(context))
        write_function_call_expression(io, context, None)
        io.write(';\n')

def write_function(io: IO[str], context: Context, function: FunctionInfo) -> None:
    context.push_scope()

    io.write('fun ')
    io.write(function.name)
    io.write('(')
    for param in function.params:
        io.write(param.name)
        io.write(' ')
        io.write(param.type.name)
        io.write(', ')
        context.add_identifier(IdentifierInfo(param.name, param.type))
    io.write(') ')
    io.write(function.return_type.name)
    io.write('\n{\n')
    context.indent_level += 1

    for _ in range(random.randint(0, 10)):
        write_statement(io, context)

    io.write(get_indent_str(context))
    io.write('return ')
    write_expression(io, context, function.return_type)
    io.write(';\n}\n')
    context.indent_level -= 1

    context.pop_scope()

def write_code(io: IO[str]) -> None:
    context = Context()
    for _ in range(random.randint(2, 10)):
        name = get_identifier(context)
        context.add_identifier(IdentifierInfo(name, TYPE_FUN))
        return_type = random.choice(EXPRESSION_TYPES)

        params: list[ParamInfo] = []
        for _ in range(random.randint(0, 3)):
            param_name = get_identifier(context)
            param_type = random.choice(EXPRESSION_TYPES)
            context.add_identifier(IdentifierInfo(param_name, param_type))
            params.append(ParamInfo(param_name, param_type))

        function = FunctionInfo(name, params, return_type)
        context.functions.append(function)

    context.clear_current_identifier_scope()
    for function in context.functions:
        context.add_identifier(IdentifierInfo(function.name, TYPE_FUN))

    for function in context.functions:
        write_function(io, context, function)

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument('-r', '--runs', type=int, default=1, help='number of runs')
    parser.add_argument('--keep-source', action='store_true', help="keep the fuzzer's generated source code")

    args = parser.parse_args()
    return args

def main() -> None:
    args = parse_args()

    src_filename = pathlib.Path('fuzzer.wip')
    out_filename = pathlib.Path('fuzzer.o')

    error_count = 0
    for _ in range(args.runs):
        with open(src_filename, 'w') as f:
            write_code(f)

        rc = run_compiler(src_filename, out_filename)
        if rc != 0:
            error_count += 1
            print(f'Error: rc={rc}')
            h = md5(src_filename)
            # src_filename.rename(f'fuzzer-{h}.wip')

    if not args.keep_source and src_filename.exists():
        os.remove(src_filename)
    if out_filename.exists():
        os.remove(out_filename)

if __name__ == '__main__':
    main()
