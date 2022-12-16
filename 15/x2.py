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

    #imax = 20
    imax = 4000000
    for iy in range(imax+1):
        excluded = []
        if iy%10000==0:print(iy)
        #print(iy)
        for s, d in sensors:
            xrange = d - abs(s[1] - iy)
            if xrange < 0:
                continue
            excluded.append([s[0] - xrange, s[0] + xrange])

        prev_len_exc = -1
        excluded.sort()
        while len(excluded) != prev_len_exc:
            prev_len_exc = len(excluded)
            for i1, exc1 in enumerate(excluded):
                for i2, exc2 in enumerate(excluded[i1+1:], start=i1+1):
                    if exc1 is None or exc2 is None: continue
                    #print('   ', i1, exc1, i2, exc2, end=' ')
                    # adjacent
                    if exc1[1]+1 == exc2[0]:
                        exc1[1] = exc2[1]
                        #print('adjacent', exc1)
                        excluded[i2] = None
                    # disjoint
                    elif exc1[1] < exc2[0]:
                        #print('disjoint')
                        pass
                    # overlap
                    else:
                        exc1[1] = max(exc1[1], exc2[1])
                        #print('overlap', exc1)
                        excluded[i2] = None


            #print ('    cleanup')
            excluded = [e for e in excluded if e is not None]

        ans = None
        if len(excluded) == 2:
            ans = (excluded[0][1] + 1, iy)
        elif len(excluded) == 1:
            if excluded[0] == 1:
                ans = (0, iy)
            elif excluded[0][1] == imax -1:
                ans = (imax, iy)

        if ans:
            print(ans)
            print(4000000*ans[0] + ans[1])
            break






def manhattan(a, b):
    return abs(b[0] - a[0]) + abs(b[1] - a[1])

main()
