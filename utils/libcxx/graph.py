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
    self.node_strings = []
    self.edge_strings = []

  def addNode(self, node):
    res = str(self.id)
    if len(self.attributes):
      res += ' ['
      attr_strs = []
      for k,v in self.attributes.iteritems():
        attr_strs += ['%s="%s"' % (k, v)]
      res += ' [ %s ]' % (', '.join(attr_strs))
    res += ';'
    self.node_strings += [res]

  def addEdge(self, n1, n2):
    res = '%s -> %s;' % (n1.id, n2.id)
    self.edge_strings += [res]

  def emit(self):
    node_definitions = '\n  '.join(self.node_strings)
    edge_list = '\n  '.join(self.edge_strings)
    return '''
digraph {name} {
{node_definitions}
{edge_list}
}    
'''.format(name=self.name, node_definitions=node_definitions, edge_list=edge_list).trim()


class DotReader(object):
  def __init__(self):
    self.graph = DirectedGraph(None)

  def abortParse(self, msg="bad input"):
    raise Exception(msg)

  def parse(self, data):
    lines = [l.trim() for l in data.splitlines() if l.trim()]
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
    parts = [l.trim() for l in raw_str.split(',') if l.trim()]
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

class DirectedGraph(object):
  def __init__(self, name, nodes):
    self.name = name
    self.nodes = defaultdict(list(nodes))

  def setName(self, n):
    self.name = n

  def addEdge(self, n1, n2):
    assert n1 in self.nodes
    assert n2 in self.nodes
    self.nodes[n1].addEdge(n2)

  def addNode(self, n):
    self.nodes.add(n)

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
