#!/usr/bin/env python
#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##
"""
sym_filter - Compare two symbol lists and output the differences.
"""

from argparse import ArgumentParser
import sys
from libcxx.sym_check import diff, util


def main():
  parser = ArgumentParser(
      description='filter symbol lists')
  group = parser.add_mutually_exclusive_group()
  group.add_argument(
      '-o', '--output', dest='output',
      help='The output file. stdout is used if not given',
      type=str, action='store', default=None)
  group.add_argument(
    '-i', '--inplace', dest='inplace', help='transform the symbols in place',
    action='store_true', default=False)
  parser.add_argument(
      '--demangle', dest='demangle', action='store_true', default=False)
  parser.add_argument(
      '--names-only', dest='names_only',
      help='Only print symbol names',
      action='store_true', default=False)
  parser.add_argument(
    'filter', metavar='filter', type=str,
    help='The filter to apply',
    choices=['stdlib', 'stdlib-versioned', 'stdlib-unversioned', 'defined', 'undefined'])
  parser.add_argument(
      'old_syms', metavar='old-syms', type=str,
      help='The file containing the old symbol list or a library')
  args = parser.parse_args()
  if args.inplace and util.is_library_file(args.old_syms):
    print("--inplace cannot be used when the input is a library file")
    sys.exit(1)

  old_syms_list = util.extract_or_load(args.old_syms)
  if args.filter == 'stdlib':
    new_syms_list, _ = util.filter_stdlib_symbols(old_syms_list)
  elif args.filter == 'stdlib-versioned':
    new_syms_list, _ = util.filter_stdlib_symbols(old_syms_list)
    new_syms_list, _ = util.filter_versioned_stdlib_symbols(new_syms_list)
  elif args.filter == 'stdlib-unversioned':
    new_syms_list, _ = util.filter_stdlib_symbols(old_syms_list)
    _, new_syms_list = util.filter_versioned_stdlib_symbols(new_syms_list)
  elif args.filter == 'defined':
    new_syms_list = [s for s in old_syms_list if s['is_defined']]
  elif args.filter == 'undefined':
    new_syms_list = [s for s in old_syms_list if not s['is_defined']]
  else:
    assert False

  output_file = None
  if args.inplace:
    output_file = args.old_syms
  elif args.output:
    output_file = args.output

  util.write_syms(new_syms_list, out=output_file, names_only=args.names_only)

if __name__ == '__main__':
  main()
