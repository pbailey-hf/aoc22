import itertools
import sys

def main():
    packets = sys.stdin.readlines()
    packets = map(str.strip, packets)
    packets = filter(len, packets)
    packets = map(eval, packets)
    packets = list(packets)
    assert len(packets) % 2 == 0
    sum = 0
    for i in range(0, len(packets)//2):
        print(f'== Pair {i+1} ==')
        if right_order(packets[i*2], packets[i*2+1]):
            print(' - Rightly ordered')
            sum += i+1
        else:
            print(' - Not rightly ordered')
    print(sum)


def right_order(items_a, items_b, depth=0):
    print(f' {depth*"  "}- Compare {items_a} vs {items_b}')

    # handle case where a is shorter than b
    if items_a is None:
        return True
    # handle case where b is shorter than a
    if items_b is None:
        return False
    if isinstance(items_a, int) and isinstance(items_b, int):
        return None if items_a == items_b else items_a < items_b
    elif isinstance(items_a, list) and isinstance(items_b, list):
        for ia, ib in itertools.zip_longest(items_a, items_b):
            if (ans := right_order(ia, ib, depth+1)) is not None:
                return ans
        return None
    elif isinstance(items_a, list):
        return right_order(items_a, [items_b], depth+1)
    elif isinstance(items_b, list):
        return right_order([items_a], items_b, depth+1)

    raise RuntimeError('unreachable')




main()
