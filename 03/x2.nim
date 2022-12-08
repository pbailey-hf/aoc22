import strutils
import std/setutils

func get(x : set[char]) : char =
  for s in x:
    return s

let upper = {'A'..'Z'}
let all = {'A'..'Z', 'a'..'z'}

var line : string
var sum = 0
var iline = 0
var lines = ["", "", ""]

while readLine(stdin, line):
  lines[iline mod 3] = strip(line)
  iline += 1
  if iline mod 3 != 0:
    continue

  var inter = all
  for line in lines:
    inter = inter*toSet(line)
  let item = get(inter)
  var val : int
  if item in upper:
    val = ord(item) - ord('A') + 27
  else:
    val = ord(item) - ord('a') + 1
  sum += val
  echo item, " ", val

echo sum



