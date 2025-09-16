#!/usr/bin/env python3

import argparse
import datetime
import math
import os
import pathlib
import random
import subprocess
import sys
from typing import IO, Iterator

SCRIPT_PATH = pathlib.Path(__file__)
ROOT_DIR = SCRIPT_PATH.parent.parent

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

TYPE_FUN = TypeInfo('fun') # TODO: generate type for each function
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
    'cast',
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

ADJECTIVES: list[str] = [
    'average',
    'blue',
    'bold',
    'caffeinated',
    'excited',
    'friendly',
    'happy',
    'methodical',
    'purple',
    'quirky',
    'sleepy',
    'sluggish',
    'small',
    'speedy',
    'super',
    'zany',
]

NOUNS: list[str] = [
    'aluminum',
    'atom',
    'bit',
    'byte',
    'coffee',
    'compiler',
    'copper',
    'electron',
    'integer',
    'keyboard',
    'kitten',
    'koala',
    'laptop',
    'memory',
    'mouse',
    'neutron',
    'process',
    'processor',
    'program',
    'proton',
    'quark',
    'sun',
    'wifi',
    'xylophone',
    'yak',
    'zebra',
]

class Scope:
    def __init__(self):
        self.identifiers: list[IdentifierInfo] = []
        self.type_weights: dict[str, float] = {}

    def copy(self) -> 'Scope':
        s = Scope()
        s.identifiers = self.identifiers[:]
        s.type_weights = {k: v for k, v in self.type_weights.items()}
        return s

    def clear(self) -> None:
        self.identifiers.clear()
        self.type_weights.clear()

    def add_identifier(self, identifier: IdentifierInfo) -> None:
        self.identifiers.append(identifier)
        type_name = identifier.type.name
        if type_name not in self.type_weights:
            self.type_weights[type_name] = 1.0
        else:
            self.type_weights[type_name] += 0.5

class Context:
    def __init__(self):
        self.functions: list[TypeInfo] = []
        self.scope_stack: list[Scope] = [Scope()]
        self.indent_level = 0
        self.expression_level = 0
        self.loop_level = 0
        self.basic_types = [
            TYPE_BOOL,
            TYPE_I8,
            TYPE_I16,
            TYPE_I32,
            TYPE_I64,
            TYPE_U8,
            TYPE_U16,
            TYPE_U32,
            TYPE_U64,
            TYPE_F32,
            TYPE_F64,
            TYPE_STR,
        ]
        self.all_types = self.basic_types[:]

        if random.randrange(6) == 0:
            self.indent_str = '\t'
        else:
            self.indent_str = ' ' * random.randint(1, 5)

    def add_identifier(self, identifier: IdentifierInfo) -> None:
        self.scope_stack[-1].add_identifier(identifier)

    def clear_current_identifier_scope(self) -> None:
        self.scope_stack[-1].clear()

    def get_current_scope_identifiers(self) -> Iterator[IdentifierInfo]:
        return iter(self.scope_stack[-1].identifiers)

    def get_current_scope_type_weights(self) -> dict[str, float]:
        return self.scope_stack[-1].type_weights

    def push_scope(self) -> None:
        self.scope_stack.append(self.scope_stack[-1].copy())

    def pop_scope(self) -> None:
        self.scope_stack.pop()

    def get_identifier_of_type(self, type: TypeInfo) -> IdentifierInfo|None:
        ids = [i for i in self.scope_stack[-1].identifiers if i.type.name == type.name]
        if len(ids) == 0:
            return None
        return random.choice(ids)

    def get_function_with_return_type(self, return_type: TypeInfo|None=None) -> TypeInfo|None:
        if return_type is None:
            functions = self.functions
        else:
            functions = [f for f in self.functions if f.return_type is not None and f.return_type.name == return_type.name]

        if len(functions) == 0:
            return None
        return random.choice(functions)

    def get_type_by_name(self, name: str) -> TypeInfo|None:
        for type in self.all_types:
            if type.name == name:
                return type
        return None

def get_type_weight(type: TypeInfo, context: Context) -> float:
    if type.is_struct:
        weight = 1.0
    else:
        weight = 5.0

    weight += context.get_current_scope_type_weights().get(type.name, 0.0)
    return weight

def get_identifier_type(context: Context) -> TypeInfo:
    all_types_weights: list[float] = [get_type_weight(t, context) for t in context.all_types]
    type = random.choices(context.all_types, all_types_weights)[0]
    return type

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
    # wrong indentation every once in a while
    if random.randrange(30) == 0:
        char = random.choice([' ', '\t'])
        return char * random.randint(0, 3)
    else:
        return context.indent_str * context.indent_level

def get_identifier(context: Context) -> str:
    invalid: set[str] = {i.name for i in context.get_current_scope_identifiers()}
    invalid |= {f.name for f in context.functions}
    invalid |= INVALID_IDENTIFIERS

    valid = False
    while not valid:
        if random.randint(0, 1) == 0:
            identifier = random.choice(ADJECTIVES) + '_' + random.choice(NOUNS)
        else:
            identifier = random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_')
            for _ in range(random.randint(0, 14)):
                identifier += random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789')

        valid = identifier not in invalid

    return identifier

def write_comment(io: IO[str], context: Context, comment: str) -> None:
    indent = get_indent_str(context)

    if random.randint(0, 1) == 0:
        io.write(indent)
        io.write(f'# {comment}\n')
    else:
        io.write(indent)
        io.write('#!\n')
        io.write(indent)
        io.write(f'  {comment}\n')
        io.write(indent)
        io.write('!#\n')

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
    r = random.randint(0, 1)
    if r == 0:
        write_expression(io, context, TYPE_BOOL)
        op = random.choice(['&', '|'])
        io.write(f' {op} ')
        write_expression(io, context, TYPE_BOOL)
    else:
        if context.expression_level > 1:
            io.write('(')

        write_expression(io, context, TYPE_I32)
        op = random.choice(['==', '!=', '<', '<=', '>', '>='])
        io.write(f' {op} ')
        write_expression(io, context, TYPE_I32)

        if context.expression_level > 1:
            io.write(')')

def write_bool_expression(io: IO[str], context: Context) -> None:
    weights: list[float] = [
        math.pow(2.0, 2.59 - context.expression_level),
        1,
        math.pow(2.0, 2.0 - context.expression_level),
        1,
    ]
    r = random.choices([0, 1, 2, 3], weights)[0]

    match r:
        case 0:
            write_bool_binary_expression(io, context)
        case 1:
            ok = write_identifier_expression(io, context, TYPE_BOOL)
            if not ok:
                write_bool_literal(io)
        case 2:
            ok = write_function_call_expression(io, context, TYPE_BOOL)
            if not ok:
                write_bool_literal(io)
        case 3:
            write_bool_literal(io)
        case _:
            assert False, f'Unexpected value: {r}'

def write_int_literal(io: IO[str], type: TypeInfo) -> None:
    r = random.randrange(0, 2)
    if r == 0:
        i = random.choice([
            '0b0',
            '0o0',
            '0',
            '0x0',
            # '0xfeedface',
        ])
    else:
        if type.is_signed:
            x = 1 << (type.size - 1)
            lower_bound = -x + 1 # TODO: take away the "+ 1" after compiler bug is fixed
            upper_bound = x - 1
        else:
            lower_bound = 0
            upper_bound = (1 << type.size) - 1
        n = random.randint(lower_bound, upper_bound)
        f = random.choice([bin, oct, str, hex])
        i = f(n)

    io.write(i)

def write_int_binary_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    write_expression(io, context, type)

    op = random.choice(['+', '-', '*', '&', '|', '^'])
    io.write(f' {op} ')

    write_expression(io, context, type)

def write_int_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    weights: list[float] = [
        math.pow(2.0, 2.59 - context.expression_level),
        1,
        math.pow(2.0, 2.0 - context.expression_level),
        1,
    ]
    r = random.choices([0, 1, 2, 3], weights)[0]

    # test implicit casts
    if type.size > 8 and random.randrange(4) == 0 and False: # TODO: enable when compiler bug is fixed
        new_type_choices = [
            t
            for t in INT_TYPES
            if t.is_signed == type.is_signed and t.size < type.size
        ]
        new_type = random.choice(new_type_choices)
    else:
        new_type = type

    match r:
        case 0:
            write_int_binary_expression(io, context, new_type)
        case 1:
            ok = write_identifier_expression(io, context, new_type)
            if not ok:
                write_int_literal(io, new_type)
        case 2:
            ok = write_function_call_expression(io, context, new_type)
            if not ok:
                write_int_literal(io, new_type)
        case 3:
            write_int_literal(io, new_type)
        case _:
            assert False, f'Unexpected value: {r}'

def write_float_literal(io: IO[str], type: TypeInfo) -> None:
    if random.randrange(5) == 0:
        n = 0.0
    else:
        n = random.random() * random.randint(1, 1_000_000_000)

    if random.randrange(2) == 0:
        n = -n

    io.write(str(n))

def write_float_binary_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    write_expression(io, context, type)

    op = random.choice(['+', '-', '*', '/', '%'])
    io.write(f' {op} ')

    write_expression(io, context, type)

def write_float_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    weights: list[float] = [
        math.pow(2.0, 2.59 - context.expression_level),
        1,
        math.pow(2.0, 2.0 - context.expression_level),
        1,
    ]
    r = random.choices([0, 1, 2, 3], weights)[0]

    match r:
        case 0:
            write_float_binary_expression(io, context, type)
        case 1:
            ok = write_identifier_expression(io, context, type)
            if not ok:
                write_float_literal(io, type)
        case 2:
            ok = write_function_call_expression(io, context, type)
            if not ok:
                write_float_literal(io, type)
        case 3:
            write_float_literal(io, type)
        case _:
            assert False, f'Unexpected value: {r}'

REALLY_LONG_STRING = 'This is a ' + ('really, ' * 20) + 'long string.'

def write_str_literal(io: IO[str]) -> None:
    io.write('"')

    r = random.randint(0, 5)
    if r == 0:
        io.write(random.choice([
            '',
            ' ',
            '\t',
            'café',
            REALLY_LONG_STRING,
        ] + list(INVALID_IDENTIFIERS)))
    elif r == 1:
        io.write(random.choice(ADJECTIVES))
        io.write(' ')
        io.write(random.choice(NOUNS))
    else:
        for _ in range(random.randint(1, 12)):
            s = random.choice('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()-=_+πé\'"') # TODO: add \ when compiler bug is fixed
            if s in ('"', '\\'):
                s = '\\' + s
            io.write(s)

    io.write('"')

def write_str_expression(io: IO[str], context: Context) -> None:
    weights: list[float] = [
        1,
        math.pow(2.0, 2.0 - context.expression_level),
        1,
    ]
    r = random.choices([0, 1, 2], weights)[0]

    match r:
        case 0:
            ok = write_identifier_expression(io, context, TYPE_STR)
            if not ok:
                write_str_literal(io)
        case 1:
            ok = write_function_call_expression(io, context, TYPE_STR)
            if not ok:
                write_str_literal(io)
        case 2:
            write_str_literal(io)
        case _:
            assert False, f'Unexpected value: {r}'

def write_struct_init_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    assert type.is_struct, f"Type '{type.name}' is not a struct"

    io.write(type.name)
    io.write('\n')
    io.write(get_indent_str(context))
    io.write('{\n')

    context.indent_level += 1

    for member in type.members:
        io.write(get_indent_str(context))
        io.write(member.name)
        io.write(': ')
        write_expression(io, context, member.type)
        io.write(',\n')

    context.indent_level -= 1

    io.write(get_indent_str(context))
    io.write('}')

def write_struct_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    assert type.is_struct, f"Type '{type.name}' is not a struct"

    weights: list[float] = [
        1,
        1 * math.exp(-context.expression_level),
    ]
    r = random.choices([0, 1], weights)[0]

    match r:
        case 0:
            ok = write_identifier_expression(io, context, type)
            if not ok:
                write_struct_init_expression(io, context, type)
        case 1:
            write_struct_init_expression(io, context, type)
        case _:
            assert False, f'Unexpected value: {r}'

def write_range_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    context.expression_level += 1

    write_expression(io, context, type)
    io.write(random.choice(['..', '..<']))
    write_expression(io, context, type)

    context.expression_level -= 1

def write_expression(io: IO[str], context: Context, type: TypeInfo) -> None:
    context.expression_level += 1

    if type.name == 'bool':
        write_bool_expression(io, context)
    elif type.is_int:
        write_int_expression(io, context, type)
    elif type.is_float:
        write_float_expression(io, context, type)
    elif type.name == 'str':
        write_str_expression(io, context)
    elif type.is_struct:
        write_struct_expression(io, context, type)
    else:
        assert False, f"Unexpected type '{type.name}'"

    context.expression_level -= 1

def write_variable_declaration(io: IO[str], context: Context) -> None:
    name = get_identifier(context)
    type = get_identifier_type(context)

    if random.randrange(20) == 0:
        write_comment(io, context, f'initializing {name}')

    io.write(get_indent_str(context))
    io.write('var ')
    io.write(name)
    io.write(' ')
    io.write(type.name)
    io.write(' = ')
    write_expression(io, context, type)
    io.write(';\n')

    context.add_identifier(IdentifierInfo(name, type))

def write_block(io: IO[str], context: Context) -> None:
    if random.randrange(2) == 0:
        io.write('\n')
        io.write(get_indent_str(context))
    else:
        io.write(' ')

    io.write('{\n')
    context.indent_level += 1

    context.push_scope()
    for _ in range(random.randint(1, 5)): # TODO: use range 0-5 when compiler bug is fixed
        write_statement(io, context)
        if random.randrange(5) == 0:
            io.write('\n')
    context.pop_scope()

    context.indent_level -= 1
    io.write(get_indent_str(context))
    io.write('}\n')

def write_if_statement(io: IO[str], context: Context) -> None:
    io.write(get_indent_str(context))
    io.write('if ')
    write_expression(io, context, TYPE_BOOL)
    write_block(io, context)

    for _ in range(random.randint(0, 3)):
        io.write(get_indent_str(context))
        io.write('elif ')
        write_expression(io, context, TYPE_BOOL)
        write_block(io, context)

    if random.randint(0, 1) == 0:
        io.write(get_indent_str(context))
        io.write('else')
        write_block(io, context)

def write_while_statement(io: IO[str], context: Context) -> None:
    io.write(get_indent_str(context))
    io.write('while ')
    write_expression(io, context, TYPE_BOOL)
    context.loop_level += 1
    write_block(io, context)
    context.loop_level -= 1

def write_for_statement(io: IO[str], context: Context) -> None:
    iter_name = get_identifier(context)

    iter_type = random.choice(INT_TYPES)
    include_index = random.randrange(2) == 0

    io.write(get_indent_str(context))
    io.write('for ')
    io.write(iter_name)
    io.write(f' {iter_type.name}')
    if include_index:
        index_name = get_identifier(context)
        while index_name == iter_name:
            index_name = get_identifier(context)
        io.write(f', {index_name} usize')
    io.write(' in ')
    write_range_expression(io, context, iter_type)

    context.push_scope()
    context.add_identifier(IdentifierInfo(iter_name, iter_type))
    if include_index:
        context.add_identifier(IdentifierInfo(index_name, TYPE_USIZE))
    context.loop_level += 1
    write_block(io, context)
    context.loop_level -= 1
    context.pop_scope()

def write_statement(io: IO[str], context: Context) -> None:
    scope_level = len(context.scope_stack)
    weights: list[float] = [
        math.exp(2.8 - scope_level),
        math.exp(2.8 - scope_level),
        math.exp(2.8 - scope_level),
        6,
        6 * math.exp(-0.4 * scope_level),
        3 * math.exp(1.1 * (1 - scope_level)),
    ]
    if context.loop_level > 0:
        weights.extend([1, 1])
    r = random.choices(list(range(len(weights))), weights)[0]

    match r:
        case 0:
            write_if_statement(io, context)
        case 1:
            write_while_statement(io, context)
        case 2:
            write_for_statement(io, context)
        case 3:
            write_variable_declaration(io, context)
        case 4:
            io.write(get_indent_str(context))
            write_function_call_expression(io, context, None)
            io.write(';\n')
        case 5:
            write_block(io, context)
        case 6:
            io.write(get_indent_str(context))
            io.write('break;\n')
        case 7:
            io.write(get_indent_str(context))
            io.write('continue;\n')
        case _:
            assert False, f'Unexpected value: {r}'

def write_function(io: IO[str], context: Context, function: TypeInfo) -> None:
    assert function.return_type is not None

    if random.randrange(3) == 0:
        write_comment(io, context, 'This function does things...')

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

def write_struct_definition(io: IO[str], context: Context, struct: TypeInfo) -> None:
    io.write('struct ')
    io.write(struct.name)
    io.write('\n{\n')
    context.indent_level += 1

    for member in struct.members:
        io.write(get_indent_str(context))
        io.write(member.name)
        io.write(' ')
        io.write(member.type.name)
        io.write(',\n')

    context.indent_level -= 1
    io.write('}\n')

def write_code(io: IO[str]) -> None:
    context = Context()

    all_types_weights: list[float] = [5.0] * len(context.all_types)
    structs: list[IdentifierInfo] = []
    for _ in range(random.randint(2, 5)):
        name = get_identifier(context)
        context.push_scope()
        members: list[IdentifierInfo] = []
        for _ in range(int(random.normalvariate(5, 2))):
            member_name = get_identifier(context)
            member_type = random.choices(context.all_types, all_types_weights)[0]
            context.add_identifier(IdentifierInfo(member_name, member_type))
            member = IdentifierInfo(member_name, member_type)
            members.append(member)
        context.pop_scope()
        struct_type = TypeInfo(name, is_struct=True, members=members)
        struct = IdentifierInfo(name, TYPE_TYPE)
        context.add_identifier(struct)
        context.all_types.append(struct_type)
        all_types_weights.append(1.0)
        structs.append(struct)

    for _ in range(random.randint(2, 10)):
        name = get_identifier(context)
        context.add_identifier(IdentifierInfo(name, TYPE_FUN))
        return_type = random.choices(context.all_types, all_types_weights)[0]

        params: list[IdentifierInfo] = []
        for _ in range(random.randint(0, 3)):
            param_name = get_identifier(context)
            param_type = random.choices(context.all_types, all_types_weights)[0]
            param = IdentifierInfo(param_name, param_type)
            context.add_identifier(param)
            params.append(param)

        function = TypeInfo(name, is_fun=True, params=params, return_type=return_type)
        context.functions.append(function)

    # clear identifiers to get rid of functions params
    context.clear_current_identifier_scope()

    # add back functions and structs
    for function in context.functions:
        context.add_identifier(IdentifierInfo(function.name, function))
    for struct in structs:
        context.add_identifier(struct)

    items = list(context.get_current_scope_identifiers())
    random.shuffle(items)
    for i, item in enumerate(items):
        type = context.get_type_by_name(item.name)
        if type is not None and type.is_struct:
            write_struct_definition(io, context, type)
        elif item.type.is_fun:
            write_function(io, context, item.type)

        if i != len(items) - 1:
            io.write('\n')

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument('--keep-source', action='store_true', help="keep the fuzzer's generated source code")
    parser.add_argument('-r', '--runs', type=int, default=None, help='number of runs')
    parser.add_argument('-t', '--time', type=int, default=None, help='max time to run (in seconds)')

    args = parser.parse_args()
    return args

def main() -> int:
    args = parse_args()

    start = datetime.datetime.now()
    out_dir = pathlib.Path(start.strftime('fuzzer_%Y-%m-%d_%H-%M-%S'))
    os.makedirs(out_dir, exist_ok=True)
    src_filename = pathlib.Path('fuzzer.wip')
    out_filename = pathlib.Path('fuzzer.o')

    max_time = None if args.time is None else datetime.timedelta(seconds=args.time)
    total_runs = 0
    error_count = 0
    while True:
        if args.runs is not None and total_runs >= args.runs:
            break
        time_diff = datetime.datetime.now() - start
        if max_time is not None and time_diff >= max_time:
            break

        with open(src_filename, 'w') as f:
            write_code(f)

        rc = run_compiler(src_filename, out_filename)
        if rc != 0:
            error_count += 1
            print(f'Error: rc={rc}')
            src_filename.rename(out_dir / f'fuzzer-{error_count}.wip')

        total_runs += 1

    total_time = datetime.datetime.now() - start
    print(f'Runs: {total_runs}')
    print(f'Errors: {error_count}')
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
