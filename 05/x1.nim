import strutils
import std/setutils

func get(x : string) : set[uint16] =
  let parts = x.split("-")
  let a = cast[uint16](parseUInt(parts[0]))
  let b = cast[uint16](parseUInt(parts[1]))
  return {a .. b}

var line : string
var linestack : seq[string]
var sum = 0
var nstacks : int
while readLine(stdin, line):
  if line.strip[0] == '1':
    nstacks = line.splitWhitespace.len
    break
  linestack.add(line)

var stacks = newSeq[seq[char]](nstacks)
for iline in countdown(linestack.len-1, 0):
  for istack in 0..nstacks:
    let index = 1 + istack*4
    if index >= linestack[iline].len:
      continue
    let c = linestack[iline][index]
    if not (c in {'A'..'Z'}):
      continue
    stacks[istack].add(c)

discard stdin.readLine
while readLine(stdin, line):
  let parts = line.split
  let count = parseInt(parts[1])
  let ifrom = parseInt(parts[3])-1
  let ito = parseInt(parts[5])-1
  for i in 0..<count:
    stacks[ito].add(stacks[ifrom].pop)

for i in 0..<stacks.len:
  stdout.write(stacks[i][^1])
echo()


