import strutils
import std/setutils

func get(x : string) : set[uint16] =
  let parts = x.split("-")
  let a = cast[uint16](parseUInt(parts[0]))
  let b = cast[uint16](parseUInt(parts[1]))
  return {a .. b}

var line : string
var sum = 0
while readLine(stdin, line):
  line = strip(line)
  let parts = line.split(",")
  let a = get(parts[0])
  let b = get(parts[1])
  if (a*b).len > 0:
    sum += 1

echo sum



