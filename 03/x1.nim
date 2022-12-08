import strutils
import std/setutils

func get(x : set[char]) : char =
  for s in x:
    return s

let upper = {'A'..'Z'}

var line : string
var sum = 0
while readLine(stdin, line):
  line = strip(line)
  let mid = line.len div 2
  let a : set[char] = toSet(line[0..<mid])
  let b : set[char] = toSet(line[mid..<line.len])
  let item = get(a*b)
  var val : int
  if item in upper:
    val = ord(item) - ord('A') + 27
  else:
    val = ord(item) - ord('a') + 1
  sum += val
  echo item, " ", val

echo sum



