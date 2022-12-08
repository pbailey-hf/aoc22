import strutils
import sequtils
import std/setutils

var line : string
const codelen = 14
while readLine(stdin, line):
  line = line.strip
  for start in 0..<line.len - codelen:
    let code = toSet(line[start..<start + codelen])
    if code.len == codelen:
      echo line, " ", start + codelen
      break
