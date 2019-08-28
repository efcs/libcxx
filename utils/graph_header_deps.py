#!/usr/bin/env python
#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

from argparse import ArgumentParser
from ctypes.util import find_library
import distutils.spawn
import glob
import tempfile
import os
import shutil
import subprocess
import signal
import sys
import shlex
import json
import re

temp_directory_root = None
def exit_with_cleanups(status):
    if temp_directory_root is not None:
        shutil.rmtree(temp_directory_root)
    sys.exit(status)

def print_and_exit(msg):
    sys.stderr.write(msg + '\n')
    exit_with_cleanups(1)

def find_and_diagnose_missing(lib, search_paths):
    if os.path.exists(lib):
        return os.path.abspath(lib)
    if not lib.startswith('lib') or not lib.endswith('.a'):
        print_and_exit(("input file '%s' not not name a static library. "
                       "It should start with 'lib' and end with '.a") % lib)
    for sp in search_paths:
        assert type(sp) is list and len(sp) == 1
        path = os.path.join(sp[0], lib)
        if os.path.exists(path):
            return os.path.abspath(path)
    print_and_exit("input '%s' does not exist" % lib)


def execute_command(cmd, cwd=None):
    """
    Execute a command, capture and return its output.
    """
    kwargs = {
        'stdin': subprocess.PIPE,
        'stdout': subprocess.PIPE,
        'stderr': subprocess.PIPE,
        'cwd': cwd,
        'universal_newlines': True
    }
    p = subprocess.Popen(cmd, **kwargs)
    out, err = p.communicate()
    exitCode = p.wait()
    if exitCode == -signal.SIGINT:
        raise KeyboardInterrupt
    return out, err, exitCode


def execute_command_verbose(cmd, cwd=None, verbose=False):
    """
    Execute a command and print its output on failure.
    """
    out, err, exitCode = execute_command(cmd, cwd=cwd)
    if exitCode != 0 or verbose:
        report = "Command: %s\n" % ' '.join(["'%s'" % a for a in cmd])
        if exitCode != 0:
            report += "Exit Code: %d\n" % exitCode
        if out:
            report += "Standard Output:\n--\n%s--" % out
        if err:
            report += "Standard Error:\n--\n%s--" % err
        if exitCode != 0:
            report += "\n\nFailed!"
        sys.stderr.write('%s\n' % report)
        if exitCode != 0:
            exit_with_cleanups(exitCode)
    return out


def get_include_dir():
    curr_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    include_dir = os.path.join(curr_dir, 'include')
    return include_dir

def extract_edges(data):
    edge_re = re.compile('^\s+(header_\d+) -> (header_\d+);.*')
    lines = data.splitlines()
    edges = []
    for l in lines:
        m = edge_re.match(l)
        if not m:
            continue
        n1 = m.group(1)
        n2 = m.group(2)
        edges += [(n1, n2)]
    return edges

def detect_cycles(edge_list):
    seen = {}
    in_rec = {}
    remaining = set()
    visited = {}
    for n1, n2 in edge_list:
        remaining.add(n1)
        remaining.add(n2)
        seen[n1] = False
        seen[n2] = False
        in_rec[n1] = False
        in_rec[n2] = False
        visited[n1] = False
        visited[n2] = False
    remaining = list(remaining)
    for i in range(0, len(remaining)):
        n = remaining[i]

        seen.add(n)
        for n1, n2 in edge_list:
            if n1 != n:
                continue



def process_dot_data(data):
    include_path = get_include_dir()
    node_re = re.compile('^\s+(header_\d+) \[.*label="([^"]+)"\];.*')
    edge_re = re.compile('^\s+(header_\d+) -> (header_\d+);.*')
    lines = data.splitlines()
    wanted_nodes = []
    unwanted_lines = []
    edges = []
    for l in lines:
        m = node_re.match(l)
        if not m:
            continue
        node_name = m.group(1)
        file_name = m.group(2)
        if ('/' + file_name).startswith(include_path):
            wanted_nodes += [node_name]
        else:
            print('inc_path = %s' % include_path)
            print('file_name = %s' % file_name)
            unwanted_lines += [l]
    for l in lines:
        m = edge_re.match(l)
        if not m:
            continue
        n1 = m.group(1)
        n2 = m.group(2)
        if n1 not in wanted_nodes or n2 not in wanted_nodes:
            unwanted_lines += [l]
    new_graph = []
    for l in lines:
        if l not in unwanted_lines:
            new_graph += [l]
    return '\n'.join(new_graph)


def canonicalize_local_nodes(data):
    include_path = get_include_dir()
    return data.replace(include_path[1:] + '/', '')


def get_headers():
    headers = []
    include_dir = get_include_dir()
    for fname in os.listdir(include_dir):
        f = os.path.join(include_dir, fname)

        if not os.path.isfile(f):
            continue
        base, ext = os.path.splitext(fname)
        if (ext == '' or ext == '.h') and (not fname.startswith('__') or fname == '__config'):
            headers += [f]
    return headers

def get_base_command(commands):
    for compile_cmd in commands:
        file = compile_cmd['file']
        if not file.endswith('src/algorithm.cpp'):
            continue
        wd = compile_cmd['directory']
        cmd_str = compile_cmd['command']
        cmd = shlex.split(cmd_str)
        out_arg = cmd.index('-o')
        del cmd[out_arg]
        del cmd[out_arg]
        in_arg = cmd.index('-c')
        del cmd[in_arg]
        del cmd[in_arg]
        return wd, cmd
    print_and_exit("failed to find command to build algorithm.cpp")

def main():
    parser = ArgumentParser(
        description="Generate a graph of libc++ header dependencies")
    parser.add_argument(
        '-v', '--verbose', dest='verbose', action='store_true', default=False)
    parser.add_argument(
        '-o', '--output', dest='output', required=True,
        help='The output file. stdout is used if not given',
        type=str, action='store')
    parser.add_argument(
        '--libcxx-only', dest='libcxx_only', action='store_true', default=False)
    parser.add_argument(
        'compile_commands', metavar='compile-commands-file',
        help='the compile commands database')

    args = parser.parse_args()
    if not os.path.isdir(args.output):
        print_and_exit('"%s" must point to a directory' % args.output)

    compile_commands = None
    with open(args.compile_commands, 'r') as f:
        compile_commands = json.load(f)


    global temp_directory_root
    temp_directory_root = tempfile.mkdtemp('.libcxx.header.deps')
    print('Using temp directory %s' % temp_directory_root)

    wd, base_cmd = get_base_command(compile_commands)
    headers_list = get_headers()

    for header in headers_list:
        header_name = os.path.basename(header)
        out = os.path.join(args.output, ('%s.dot' % header_name))
        input = os.path.join(temp_directory_root, ('test_%s.cpp' % header_name))
        with open(input, 'w') as f:
            f.write('#include <%s>\n\n' % header_name)
        cmd = base_cmd + ["-fsyntax-only", "-Xclang", "-dependency-dot", "-Xclang", "%s" % out, '-xc++', input]
        execute_command_verbose(cmd, cwd=wd, verbose=args.verbose)


    for fname in os.listdir(args.output):
        dot_file = os.path.join(args.output, fname)
        if not os.path.isfile(dot_file):
            continue
        data = None
        with open(dot_file, 'r') as f:
            data = f.read()
        if args.libcxx_only:
            data = process_dot_data(data)
        data = canonicalize_local_nodes(data)
        with open(dot_file, 'w') as f:
            f.write(data)


if __name__ == '__main__':
    main()
    exit_with_cleanups(0)
