# -*- Python -*- vim: set syntax=python tabstop=4 expandtab cc=80:
#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##
"""
diff - A set of functions for diff-ing two symbol lists.
"""

from libcxx.sym_check import util
from pprint import pformat

def _symbol_difference(lhs, rhs):
    lhs_names = set((n['name'] for n in lhs))
    rhs_names = set((n['name'] for n in rhs))
    diff_names = lhs_names - rhs_names
    return [n for n in lhs if n['name'] in diff_names]


def _find_by_key(sym_list, k):
    for sym in sym_list:
        if sym['name'] == k:
            return sym
    return None


def added_symbols(old, new):
    return _symbol_difference(new, old)


def removed_symbols(old, new):
    return _symbol_difference(old, new)

def are_keys_eq(old, new, ignore=[]):
    old_keys = set(old.keys()).difference(set(ignore))
    new_keys = set(new.keys()).difference(set(ignore))
    common_keys = set(old_keys).intersection(new_keys)
    for k in common_keys:
        if new[k] != old[k]:
            return False
    return True

def changed_symbols(old, new):
    changed = []
    for old_sym in old:
        if old_sym in new:
            continue
        new_sym = _find_by_key(new, old_sym['name'])
        if (new_sym is not None and not new_sym in old
                and not are_keys_eq(old_sym, new_sym, ignore=['file'])):
            changed += [(old_sym, new_sym)]
    return changed

def common_symbols(old, new):
    common = []
    for old_sym in old:
        new_sym = _find_by_key(new, old_sym['name'])
        if new_sym is not None:
            common += [(old_sym, new_sym)]
    return common

def diff(old, new):
    added = added_symbols(old, new)
    removed = removed_symbols(old, new)
    changed = changed_symbols(old, new)
    common = common_symbols(old, new)
    return added, removed, changed, common

def extract_all_names(sym_list):
    names = set([s['name'] for s in sym_list])
    return names

def _print_sym(s):
    return pformat(s, width=100000)

def report_diff(added_syms, removed_syms, changed_syms, common_syms,
                names_only=False,
                demangle=True):
    all_names = extract_all_names(list(added_syms) + removed_syms + \
      [s for (_, s) in common_syms] +
      [s for (_, s) in changed_syms])
    if demangle:
        demangled_names = util.demangle_symbols(all_names)
    else:
        demangled_names = {n:n for n in all_names}
    report = ''
    for sym in added_syms:
        report += 'Symbol added: %s\n' % demangled_names[sym['name']]
        if not names_only:
            report += '    %s\n\n' % _print_sym(sym)
    if added_syms and names_only:
        report += '\n'
    for sym in removed_syms:
        report += 'SYMBOL REMOVED: %s\n' % demangled_names[sym['name']]
        if not names_only:
            report += '    %s\n\n' % _print_sym(sym)
    if removed_syms and names_only:
        report += '\n'
    if not names_only:
        for sym_pair in changed_syms:
            old_sym, new_sym = sym_pair
            old_str = '\n    OLD SYMBOL: %s' % _print_sym(old_sym)
            new_str = '\n    NEW SYMBOL: %s' % _print_sym(new_sym)
            report += ('SYMBOL CHANGED: %s%s%s\n\n' %
                       (demangled_names[old_sym['name']],
                        old_str, new_str))

    added = bool(len(added_syms) != 0)
    abi_break = bool(len(removed_syms))
    if not names_only:
        abi_break = abi_break or len(changed_syms)
    if common_syms:
        report += 'Common symbols:\n'
        for old_sym, new_sym in common_syms:
            report += '%s\n' % demangled_names[old_sym['name']]
            if not names_only:
                report += '    %s\n' % _print_sym(old_sym)
                if old_sym != new_sym:
                    report += '    %s\n' % _print_sym(new_sym)
    if added or abi_break:
        report += 'Summary\n'
        report += '    Added:   %d\n' % len(added_syms)
        report += '    Removed: %d\n' % len(removed_syms)
        if not names_only:
            report += '    Changed: %d\n' % len(changed_syms)
        if not abi_break:
            report += 'Symbols added.'
        else:
            report += 'ABI BREAKAGE: SYMBOLS ADDED OR REMOVED!'
    else:
        report += 'Symbols match.'
    is_different = abi_break or bool(len(added_syms)) \
                   or bool(len(changed_syms))
    return report, abi_break, is_different
