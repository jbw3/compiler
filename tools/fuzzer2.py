#!/usr/bin/env python3

import argparse
import datetime
import json
import math
import os
import random
import subprocess
import sys
from pathlib import Path
from typing import Any, Callable, IO, Iterator

Constraint = Callable[['ConstExpression'], bool]

def int_floor_divide(a: int, b: int) -> int:
    d = abs(a) // abs(b)
    if (a < 0) != (b < 0):
        d = -d
    return d

def int_floor_modulo(a: int, b: int) -> int:
    m = abs(a) % abs(b)
    if a < 0:
        m = -m
    return m

class IdentifierInfo:
    def __init__(self, name: str, type: 'TypeInfo'):
        self.name = name
        self.type = type

class TypeInfo:
    def __init__(
        self,
        name: str,
        size: int = 0,
        is_struct: bool=False,
        members: list[IdentifierInfo]|None=None,
        is_fun: bool=False,
        params: list[IdentifierInfo]|None=None,
        return_type: 'TypeInfo|None'=None,
        is_int: bool = False,
        is_float: bool = False,
        is_signed: bool = False,
    ):
        self.name = name
        self.size = size
        self.is_struct = is_struct
        self.members = [] if members is None else members
        self.is_fun = is_fun
        self.params = [] if params is None else params
        self.return_type = return_type
        self.is_int = is_int
        self.is_float = is_float
        self.is_signed = is_signed

    @property
    def is_numeric(self) -> bool:
        return self.is_int or self.is_float

TYPE_BOOL = TypeInfo('bool')
TYPE_I8 = TypeInfo('i8', size=8, is_int=True, is_signed=True)
TYPE_I16 = TypeInfo('i16', size=16, is_int=True, is_signed=True)
TYPE_I32 = TypeInfo('i32', size=32, is_int=True, is_signed=True)
TYPE_I64 = TypeInfo('i64', size=64, is_int=True, is_signed=True)
TYPE_ISIZE = TypeInfo('isize', size=64, is_int=True, is_signed=True)
TYPE_U8 = TypeInfo('u8', size=8, is_int=True, is_signed=False)
TYPE_U16 = TypeInfo('u16', size=16, is_int=True, is_signed=False)
TYPE_U32 = TypeInfo('u32', size=32, is_int=True, is_signed=False)
TYPE_U64 = TypeInfo('u64', size=64, is_int=True, is_signed=False)
TYPE_USIZE = TypeInfo('usize', size=64, is_int=True, is_signed=False)
TYPE_F32 = TypeInfo('f32', size=32, is_float=True, is_signed=True)
TYPE_F64 = TypeInfo('f64', size=64, is_float=True, is_signed=True)
TYPE_STR = TypeInfo('str')
TYPE_TYPE = TypeInfo('type')

INT_TYPES: list[TypeInfo] = [
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_ISIZE,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_USIZE,
]

INVALID_IDENTIFIERS: set[str] = {
    'bool',
    'break',
    'const',
    'continue',
    'elif',
    'else',
    'f32',
    'f64',
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
    'struct',
    'type',
    'u8',
    'u16',
    'u32',
    'u64',
    'usize',
    'var',
    'while',
}

class Scope:
    def __init__(self):
        self.identifiers: list[IdentifierInfo] = []
        self.type_weights: dict[str, float] = {}
        self.struct_members_by_type: dict[str, list[tuple[str, TypeInfo]]] = {}

    def copy(self) -> 'Scope':
        s = Scope()
        s.identifiers = self.identifiers[:]
        s.type_weights = {k: v for k, v in self.type_weights.items()}
        s.struct_members_by_type = {k: v for k, v in self.struct_members_by_type.items()}
        return s

    def clear(self) -> None:
        self.identifiers.clear()
        self.type_weights.clear()
        self.struct_members_by_type.clear()

    def add_identifier(self, identifier: IdentifierInfo) -> None:
        self.identifiers.append(identifier)
        type_name = identifier.type.name
        if type_name not in self.type_weights:
            self.type_weights[type_name] = 1.0
        else:
            self.type_weights[type_name] += 0.5

        type = identifier.type
        if type.is_struct:
            for member in type.members:
                member_type_name = member.type.name
                if member_type_name not in self.struct_members_by_type:
                    self.struct_members_by_type[member_type_name] = []
                self.struct_members_by_type[member_type_name].append((member.name, type))

class TypeCategories:
    def __init__(self) -> None:
        self.basic: list[TypeInfo] = [
            TYPE_BOOL,
            TYPE_I8,
            TYPE_I16,
            TYPE_I32,
            TYPE_I64,
            TYPE_ISIZE,
            TYPE_U8,
            TYPE_U16,
            TYPE_U32,
            TYPE_U64,
            TYPE_USIZE,
            TYPE_F32,
            TYPE_F64,
            TYPE_STR,
            TYPE_TYPE,
        ]

class ConstExpression:
    def __init__(
        self,
        type: TypeInfo,
        value: Any,
        value_str: str,
        is_literal: bool,
        precedence: int = 100,
    ) -> None:
        self.type = type
        self.value = value
        self.value_str = value_str
        self.is_literal = is_literal
        self.precedence = precedence

    @staticmethod
    def get_int_max_min_constraint(type: TypeInfo) -> Constraint:
        if type.is_signed:
            x = 1 << (type.size - 1)
            lower_bound = -x
            upper_bound = x - 1
        else:
            lower_bound = 0
            upper_bound = (1 << type.size) - 1

        def f(expr: 'ConstExpression') -> bool:
            return lower_bound <= expr.value <= upper_bound

        return f

    @staticmethod
    def generate(type: TypeInfo) -> 'ConstExpression':
        if type.name == 'bool':
            expr = ConstExpression(type, False, 'false', True)
        elif type.is_int:
            expr = ConstExpression.gen_int_expr(type, ConstExpression.get_int_max_min_constraint(type))
        elif type.is_float:
            expr = ConstExpression(type, 0.1, "0.1", True)
        elif type.name == 'str':
            expr = ConstExpression(type, '', '""', True)
        elif type.name == 'type':
            expr = ConstExpression(type, {}, "struct { }", False)
        else:
            assert False, f"Unexpected type '{type.name}'"

        return expr

    @staticmethod
    def gen_int_expr(
        type: TypeInfo,
        constraint: Constraint | None = None
    ) -> 'ConstExpression':
        expr = None
        while expr is None or (constraint is not None and not constraint(expr)):
            if random.randint(0, 5) == 0:
                expr = ConstExpression.gen_int_binary_expr(type)
            else:
                expr = ConstExpression.gen_int_literal(type)

        return expr

    @staticmethod
    def gen_int_literal(type: TypeInfo) -> 'ConstExpression':
        r = random.randrange(0, 2)
        if r == 0:
            value = 0
            value_str = random.choice([
                '0b0',
                '0o0',
                '0',
                '0x0',
            ])
        else:
            if type.is_signed:
                x = 1 << 63
                lower_bound = -x
                upper_bound = x - 1
            else:
                lower_bound = 0
                upper_bound = (1 << 64) - 1
            value = random.randint(lower_bound, upper_bound)
            f = random.choice([bin, oct, str, hex])
            value_str = f(value)

        expr = ConstExpression(type, value, value_str, True)
        return expr

    @staticmethod
    def gen_int_binary_expr(type: TypeInfo) -> 'ConstExpression':
        op, precedence = random.choice([
            ('+', 8),
            ('-', 8),
            ('*', 9),
            ('/', 9),
            ('%', 9),
            ('&', 6),
            ('|', 4),
            ('^', 5),
        ])

        constraint: Constraint | None
        if op == '/' or op == '%':
            constraint = lambda e: e.value != 0
        else:
            constraint = None

        left = ConstExpression.gen_int_expr(type)
        right = ConstExpression.gen_int_expr(type, constraint)

        match op:
            case '+':
                value = left.value + right.value
            case '-':
                value = left.value - right.value
            case '*':
                value = left.value * right.value
            case '/':
                value = int_floor_divide(left.value, right.value)
            case '%':
                value = int_floor_modulo(left.value, right.value)
            case '&':
                value = left.value & right.value
            case '|':
                value = left.value | right.value
            case '^':
                value = left.value ^ right.value
            case _:
                assert False, f"Unexpected op '{op}'"

        # check precedence
        if left.precedence < precedence:
            left_str = f'({left.value_str})'
        else:
            left_str = left.value_str

        if right.precedence <= precedence:
            right_str = f'({right.value_str})'
        else:
            right_str = right.value_str

        value_str = f"{left_str} {op} {right_str}"
        expr = ConstExpression(
            type,
            value,
            value_str,
            left.is_literal and right.is_literal,
            precedence,
        )
        return expr

class SourceGenerator:
    def __init__(self) -> None:
        with open('fuzzer_data.json', 'r') as f:
            json_data = json.load(f)
        self.adjectives: list[str] = json_data['adjectives']
        self.nouns: list[str] = json_data['nouns']

        self.type_categories = TypeCategories()
        self.scope_stack: list[Scope] = [Scope()]

    def add_identifier(self, identifier: IdentifierInfo) -> None:
        self.scope_stack[-1].add_identifier(identifier)

    def get_current_scope_identifiers(self) -> Iterator[IdentifierInfo]:
        return iter(self.scope_stack[-1].identifiers)

    def push_scope(self) -> None:
        self.scope_stack.append(self.scope_stack[-1].copy())

    def pop_scope(self) -> None:
        self.scope_stack.pop()

    def create_identifier_name(self) -> str:
        invalid: set[str] = {i.name for i in self.get_current_scope_identifiers()}
        invalid |= INVALID_IDENTIFIERS

        valid = False
        while not valid:
            if random.randrange(2) == 0:
                adjective = random.choice(self.adjectives)
                noun = random.choice(self.nouns)
                if random.randrange(2) == 0:
                    identifier = adjective + '_' + noun
                else:
                    identifier = adjective + noun.title()
            else:
                identifier = random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_')
                identifier += ''.join(random.choices('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789', k=random.randint(0, 14)))

            valid = identifier not in invalid

        return identifier

    def get_new_const_type(self) -> TypeInfo:
        return random.choice(self.type_categories.basic)

    def write_code(self, io: IO[str]) -> None:
        const_infos: list[IdentifierInfo] = []

        # create constant names
        for _ in range(random.randint(0, 10)):
            name = self.create_identifier_name()
            type = self.get_new_const_type()
            info = IdentifierInfo(name, type)
            self.add_identifier(info)
            const_infos.append(info)

        for i, const_info in enumerate(const_infos):
            type = const_info.type
            expr = ConstExpression.generate(type)
            io.write('const ')
            io.write(const_info.name)
            if (type.is_numeric and expr.is_literal) or random.randint(0, 1) == 0:
                io.write(' ')
                io.write(type.name)
            io.write(' = ')
            io.write(expr.value_str)
            io.write(';\n')

            if i < len(const_infos) - 1:
                io.write('\n')

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument('--keep-source', action='store_true', help="keep the fuzzer's generated source code")
    parser.add_argument('-m', '--max-errors', type=int, default=None, help='maximum number of errors')
    parser.add_argument('-r', '--runs', type=int, default=None, help='number of runs')
    parser.add_argument('-t', '--time', type=int, default=None, help='max time to run (in seconds)')

    args = parser.parse_args()
    return args

def main() -> int:
    args = parse_args()

    start = datetime.datetime.now()
    out_dir = Path(start.strftime('fuzzer_%Y-%m-%d_%H-%M-%S'))
    os.makedirs(out_dir, exist_ok=True)
    src_filename = Path('fuzzer.wip')
    out_filename = Path('fuzzer.o')

    script_path = Path(__file__)
    root_dir = script_path.parent.parent
    compiler_path = root_dir / 'debug' / 'compiler' / 'wip'
    cmd = [
        compiler_path,
        src_filename,
        '-o',
        out_filename,
    ]

    max_time = None if args.time is None else datetime.timedelta(seconds=args.time)
    total_runs = 0
    error_count = 0
    while True:
        if args.max_errors is not None and error_count >= args.max_errors:
            break
        if args.runs is not None and total_runs >= args.runs:
            break
        if max_time is not None and datetime.datetime.now() - start >= max_time:
            break

        gen = SourceGenerator()
        with open(src_filename, 'w') as f:
            gen.write_code(f)

        proc = subprocess.run(cmd)
        rc = proc.returncode
        if rc != 0:
            error_count += 1
            print(f'Error: rc={rc}')
            src_filename.rename(out_dir / f'fuzzer-{error_count}.wip')

        total_runs += 1

    total_time = datetime.datetime.now() - start
    print(f'Runs: {total_runs:,}')
    print(f'Errors: {error_count:,}')
    print(f'Time: {total_time}')

    if error_count == 0 and out_dir.exists():
        os.removedirs(out_dir)
    if not args.keep_source and src_filename.exists():
        os.remove(src_filename)
    if out_filename.exists():
        os.remove(out_filename)

    return 0 if error_count == 0 else 1

if __name__ == '__main__':
    rc = main()
    sys.exit(rc)
