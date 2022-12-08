import strutils

type
  Outcome = enum
    Lose, Draw, Win

let outcomes = [
  [Outcome.Draw, Outcome.Win, Outcome.Lose],
  [Outcome.Lose, Outcome.Draw, Outcome.Win],
  [Outcome.Win, Outcome.Lose, Outcome.Draw],
  ]

let shapeToOutcome = [
  [2, 0, 1],
  [0, 1, 2],
  [1, 2, 0],
  ]

let pointsShape = [1, 2, 3]
let pointsOutcome = [0, 3, 6]

var line : string
var sum = 0
while readLine(stdin, line):
  line = strip(line)
  let shapes = line.split(" ");
  let theirShape = ord(shapes[0][0]) - ord('A')
  let outcome = Outcome(ord(shapes[1][0]) - ord('X'))
  let ourShape = shapeToOutcome[theirShape][ord(outcome)]
  let points = pointsShape[ourShape] + pointsOutcome[ord(outcome)]
  sum += points
  echo line, " ", outcome, " ", points

echo sum
