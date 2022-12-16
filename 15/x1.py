import sys

def main():
    sensors = []
    beacons = set()
    for line in sys.stdin:
        s, b = line.split(':')
        s = tuple(int(x[2:].strip(',')) for x in s.split()[-2:])
        b = tuple(int(x[2:].strip(',')) for x in b.split()[-2:])
        sensors.append((s, manhattan(s,b)))
        beacons.add(b)

    iy = 2000000
    nobeacon = set()
    for s, d in sensors:
        xrange = d - abs(s[1] - iy)
        print(s, d, xrange)
        for ix in range(s[0] - xrange, s[0] + xrange + 1):
            pos = (ix, iy)
            #print('  ', pos, manhattan(s, pos))
            assert manhattan(s, pos) <= d
            if pos not in beacons:
                nobeacon.add(pos)

    print(len(nobeacon))


def manhattan(a, b):
    return abs(b[0] - a[0]) + abs(b[1] - a[1])

main()
