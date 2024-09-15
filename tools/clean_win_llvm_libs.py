import os
import subprocess

def read_cmake(cmake_filename: str) -> tuple[str, str, list[str]]:
    prefix = ''
    postfix = ''
    lib_lines: list[str] = []
    with open(cmake_filename, 'r') as f:
        # prefix
        for line in f:
            prefix += line
            if 'set(llvm_libs' in line:
                break

        # libs
        line = f.readline()
        while line != '' and line.strip() != ')':
            lib_lines.append(line)
            line = f.readline()

        # postfix
        while line != '':
            postfix += line
            line = f.readline()

    return prefix, postfix, lib_lines

def gen_cmake(cmake_filename: str, prefix: str, postfix: str, lib_lines: list[str]) -> None:
    with open(cmake_filename, 'w') as f:
        f.write(prefix)
        for line in lib_lines:
            f.write(line)
        f.write(postfix)

def main() -> None:
    tools_dir = os.path.dirname(__file__)
    top_dir = os.path.abspath(os.path.join(tools_dir, '..'))
    src_dir = os.path.join(top_dir, 'src')
    build_dir = os.path.join(top_dir, 'build')

    cmake_filename = os.path.join(src_dir, 'CMakeLists.txt')
    prefix, postfix, lib_lines = read_cmake(cmake_filename)

    os.chdir(build_dir)

    i = 0
    while i < len(lib_lines):
        new_lib_lines = lib_lines[:]
        removed_line = new_lib_lines.pop(i)
        gen_cmake(cmake_filename, prefix, postfix, new_lib_lines)

        # build
        rc = subprocess.call(['cmake', '--build', '.'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        removed_line = removed_line.strip()
        if rc == 0:
            lib_lines = new_lib_lines
            print(f'{removed_line} is not necessary')
        else:
            i += 1
            print(f'{removed_line} is necessary')

    gen_cmake(cmake_filename, prefix, postfix, lib_lines)

if __name__ == '__main__':
    main()
