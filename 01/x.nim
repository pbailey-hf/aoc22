import algorithm
import strutils
import sequtils

var elves = @[0]

var line : string
while readLine(stdin, line):
  line = strip(line)
  if line == "":
    # skip multiple blank lines
    if elves[^1] != 0:
      elves.add(0)
  else:
    elves[^1] += parseInt(line)

sort(elves)
echo elves
echo foldl(elves[^3..^1], a+b)
