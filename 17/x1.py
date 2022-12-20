import sys
import time

WIDTH = 7

def main():
    dirs = sys.stdin.readline().strip()
    print(dirs)
    idir = 0
    shapes = '-+j|o'
    ishape = 0

    rocks = set()
    rock = Rock(shapes[0], 0)
    irock = 1
    maxrock = 0
    time_start = time.monotonic()
    #nrocks = 1000000000000
    nrocks = 2022
    while irock <= nrocks:
        if rock.imove % 2 == 0:
            rock.move(dirs[idir%len(dirs)], rocks)
            idir += 1
        elif not rock.move('d', rocks):
            maxrock = max(maxrock, max(p[0] for p in rock.points))
            rocks |= set(rock.points)
            rock = Rock(shapes[irock%len(shapes)], maxrock)
            irock += 1
            if irock % 10000 == 0:
                rocks = cleanup(rocks)
                print(f'{irock/nrocks*100:.2f}%')
                duration = time.monotonic() - time_start
                rps = irock/duration
                print(f'ETA {(nrocks-irock)/rps/60} min; {len(rocks)}')

    print(maxrock)

def cleanup(rocks):
    rocks = sorted(rocks, key=lambda p: (-p[0], p[1]))
    xs = set()
    for i, (y, x) in enumerate(rocks):
        xs.add(x)
        if len(xs) == WIDTH:
            break
    return set(rocks[:i])



def print_rocks(rocks):
    rocks = sorted(rocks, key=lambda p: (-p[0], p[1]))
    print(rocks)

class Rock:
    def __init__(self, shape, maxrock):
        self.imove = 0
        bottom = maxrock + 4
        left = 2
        if shape == '-':
            self.points = [
                (bottom, left),
                (bottom, left+1),
                (bottom, left+2),
                (bottom, left+3),
                ]
        elif shape == '+':
            self.points = [
                (bottom+2, left+1),
                (bottom+1, left),
                (bottom+1, left+1),
                (bottom+1, left+2),
                (bottom, left+1),
                ]
        elif shape == 'j':
            self.points = [
                (bottom+2, left+2),
                (bottom+1, left+2),
                (bottom, left),
                (bottom, left+1),
                (bottom, left+2),
                ]
        elif shape == '|':
            self.points = [
                (bottom+3, left),
                (bottom+2, left),
                (bottom+1, left),
                (bottom+0, left),
                ]
        elif shape == 'o':
            self.points = [
                (bottom+1, left),
                (bottom+1, left+1),
                (bottom, left),
                (bottom, left+1),
                ]
        else:
            raise RuntimeError(f'unknown shape {shape}')

    def move(self, direction, rocks):
        assert self.imove%2 == 0 or direction == 'd'
        self.imove += 1

        if direction == 'd':
            vec = (-1, 0)
        elif direction == '<':
            vec = (0, -1)
        elif direction == '>':
            vec = (0, 1)
        else:
            raise RuntimeError(f'unknown direction {direction}')

        newpts = [(p[0] + vec[0], p[1] + vec[1]) for p in self.points]
        xmin = min(p[1] for p in newpts)
        xmax = max(p[1] for p in newpts)
        ymin = min(p[0] for p in newpts)
        #print('move', direction, vec, self.points, newpts)

        if xmin < 0 or xmax >= WIDTH or ymin < 1:
            return False

        if set(newpts).intersection(rocks):
            return False

        self.points = newpts
        return True


main()
