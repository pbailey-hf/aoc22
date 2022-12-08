import strutils
import sequtils
import std/setutils
import std/tables

type
  Dir* = object
    name*: string
    dirs*: Table[string, ref Dir]
    size*: int
    parent*: ref Dir

var root = new Dir
root.name = "/"

echo root[]
var cwd = root
var alldirs = @[root]

var line : string
while readLine(stdin, line):
  line = line.strip
  echo line
  if line.startsWith('$'):
    let parts = line.split
    let cmd = parts[1]
    if cmd == "cd":
      let dirname = parts[2]
      if dirname == "..":
        cwd = cwd.parent
      elif dirname == "/":
        cwd = root
      else:
        cwd = cwd.dirs[dirname]
  else:
    let parts = line.split
    if parts[0] == "dir":
      let dirname = parts[1]
      var dir = new Dir
      dir.name = dirname
      dir.parent = cwd
      cwd.dirs[dirname] = dir
      alldirs.add(dir)
    else:
      cwd.size += parseInt(parts[0])

  echo cwd[]

func size(dir: Dir) : int =
  result = dir.size
  for subdir in dir.dirs.values:
    result += size(subdir[])

const total = 70000000
const needed = 30000000
let current = size(root[])
var best = int.high

for dir in alldirs:
  let s = size(dir[])
  if current - s <= total - needed and s < best:
    best = s
echo best
