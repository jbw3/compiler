#!/usr/bin/env python3

from pathlib import Path
import subprocess

def main() -> None:
    script_dir = Path(__file__).parent
    root_dir = script_dir.parent
    build_dir = root_dir / 'debug'
    api_dir = root_dir / 'src' / 'compilerapi'

    # run ctest
    subprocess.check_call(
        ['ctest', '--quiet'],
        env={'LLVM_PROFILE_FILE': 'default.profraw'},
        cwd=build_dir,
    )

    prof_raw_path = root_dir / 'default.profraw'

    subprocess.check_call(
        ['llvm-profdata-20', 'merge', '-sparse', prof_raw_path, '-o', 'default.profdata'],
        cwd=build_dir,
    )

    subprocess.check_call(
        [
            'llvm-cov-20', 'show',
            '--instr-profile', 'default.profdata',
            'unittests/unittests',
            '--sources', api_dir,
            '-format=html',
            '-output-dir=../coverage-report',
        ],
        cwd=build_dir,
    )

    with open(build_dir / 'lcov.info', 'wb') as lcov_info:
        subprocess.check_call(
            [
                'llvm-cov-20', 'export',
                '--instr-profile', 'default.profdata',
                'unittests/unittests',
                '--sources', api_dir,
                '-format=lcov',
            ],
            stdout=lcov_info,
            cwd=build_dir,
        )

if __name__ == '__main__':
    main()
