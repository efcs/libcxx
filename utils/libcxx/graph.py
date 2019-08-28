#===----------------------------------------------------------------------===##
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#===----------------------------------------------------------------------===##

import platform
import os
from collections import defaultdict
import re
import libcxx.util


class DotEmitter(object):
  def __init__(self, name):
    self.name = name
    self.node_strings = {}
    self.edge_strings = []

  def addNode(self, node):
    res = str(node.id)
    if len(node.attributes):
      attr_strs = []
      for k,v in node.attributes.iteritems():
        attr_strs += ['%s="%s"' % (k, v)]
      res += ' [ %s ]' % (', '.join(attr_strs))
    res += ';'
    assert node.id not in self.node_strings
    self.node_strings[node.id] = res

  def addEdge(self, n1, n2):
    res = '%s -> %s;' % (n1.id, n2.id)
    self.edge_strings += [res]

  def node_key(self, n):
    id = n.id
    assert id.startswith('\w*\d+')

  def emit(self):
    node_definitions_list = []
    sorted_keys = self.node_strings.keys()
    sorted_keys.sort()
    for k in sorted_keys:
      node_definitions_list += [self.node_strings[k]]
    node_definitions = '\n  '.join(node_definitions_list)
    edge_list = '\n  '.join(self.edge_strings)
    return '''
digraph "{name}" {{
  {node_definitions}
  {edge_list}
}}    
'''.format(name=self.name, node_definitions=node_definitions, edge_list=edge_list).strip()


class DotReader(object):
  def __init__(self):
    self.graph = DirectedGraph(None)

  def abortParse(self, msg="bad input"):
    raise Exception(msg)

  def parse(self, data):
    lines = [l.strip() for l in data.splitlines() if l.strip()]
    maxIdx = len(lines)
    idx = 0
    if not self.parseIntroducer(lines[idx]):
      self.abortParse('failed to parse introducer')
    idx += 1
    while idx < maxIdx:
      if self.parseNodeDefinition(lines[idx]) or self.parseEdgeDefinition(lines[idx]):
        idx += 1
        continue
      else:
        break
    if idx == maxIdx or not self.parseCloser(lines[idx]):
      self.abortParse("no closing } found")
    return self.graph

  def parseEdgeDefinition(self, l):
    edge_re = re.compile('^\s*(\w+)\s+->\s+(\w+);\s*$')
    m = edge_re.match(l)
    if not m:
      return False
    n1 = m.group(1)
    n2 = m.group(2)
    self.graph.addEdge(n1, n2)
    return True

  def parseAttributes(self, raw_str):
    attribute_re = re.compile('^\s*(\w+)="([^"]+)"')
    parts = [l.strip() for l in raw_str.split(',') if l.strip()]
    attribute_dict = {}
    for a in parts:
      m = attribute_re.match(a)
      if not m:
        self.abortParse('Bad attribute "%s"' % a)
      attribute_dict[m.group(1)] = m.group(2)
    return attribute_dict

  def parseNodeDefinition(self, l):
    node_definition_re = re.compile('^\s*(\w+)\s+\[([^\]]+)\]\s*;\s*$')
    m = node_definition_re.match(l)
    if not m:
      return False
    id = m.group(1)
    attributes = self.parseAttributes(m.group(2))
    n = Node(id, edges=[], attributes=attributes)
    self.graph.addNode(n)
    return True

  def parseIntroducer(self, l):
    introducer_re = re.compile('^\s*digraph "([^"]+)"\s+{\s*$')
    m = introducer_re.match(l)
    if not m:
      return False
    self.graph.setName(m.group(1))
    return True

  def parseCloser(self, l):
    closer_re = re.compile('^\s*}\s*$')
    m = closer_re.match(l)
    if not m:
      return False
    return True

class Node(object):
  def __init__(self, id, edges=[], attributes={}):
    self.id = id
    self.edges = set(edges)
    self.attributes = dict(attributes)

  def addEdge(self, dest):
    self.edges.add(dest)

  def __eq__(self, another):
    if isinstance(another, str):
      return another == self.id
    return hasattr(another, 'id') and self.id == another.id

  def __hash__(self):
    return hash(self.id)

  def __str__(self):
    res = self.id
    if len(self.attributes):
      attr = []
      for k,v in self.attributes.iteritems():
        attr += ['%s="%s"' % (k, v)]
      res += ' [%s ]' % (', '.join(attr))
    return res

  def __repr__(self):
    return '%s' % self

class DirectedGraph(object):
  def __init__(self, name=None, nodes=None):
    self.name = name
    self.nodes = set() if nodes is None else set(nodes)

  def setName(self, n):
    self.name = n

  def _getNode(self, n_or_id):
    if isinstance(n_or_id, Node):
      return n_or_id
    return self.getNode(n_or_id)

  def getNode(self, str_id):
    for s in self.nodes:
      if s.id == str_id:
        return s
    return None

  def getNodeByLabel(self, l):
    found = None
    for s in self.nodes:
      if s.attributes['label'] == l:
        assert found is None
        found = s
    return found

  def addEdge(self, n1, n2):
    assert isinstance(n1, str)
    assert isinstance(n2, str)
    assert n1 in self.nodes
    assert n2 in self.nodes
    self.getNode(n1).addEdge(self.getNode(n2))

  def addNode(self, n):
    self.nodes.add(n)

  def removeNode(self, n):
    n = self._getNode(n)
    for other_n in self.nodes:
      if other_n == n:
        continue
      new_edges = set()
      for e in other_n.edges:
        if e != n:
          new_edges.add(e)
      other_n.edges = new_edges
    self.nodes.remove(n)

  def toDot(self):
    dot = DotEmitter(self.name)
    for n in self.nodes:
      dot.addNode(n)
      for ndest in n.edges:
        dot.addEdge(n, ndest)
    return dot.emit()

  @staticmethod
  def fromDot(str):
    reader = DotReader()
    graph = reader.parse(str)
    return graph

  @staticmethod
  def fromDotFile(fname):
    with open(fname, 'r') as f:
      return DirectedGraph.fromDot(f.read())

  def toDotFile(self, fname):
    with open(fname, 'w') as f:
      f.write(self.toDot())

  def __repr__(self):
    return self.toDot()
