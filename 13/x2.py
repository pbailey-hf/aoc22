import functools
import itertools
import sys

def main():
    packets = sys.stdin.readlines()
    packets = map(str.strip, packets)
    packets = filter(len, packets)
    packets = map(eval, packets)
    packets = list(packets)
    assert len(packets) % 2 == 0
    dividers = [ [[2]], [[6]] ]
    packets.extend(dividers)
    packets.sort(key=functools.cmp_to_key(cmp))
    print('\n'.join(str(p) for p in packets))
    prod = 1
    for i, p in enumerate(packets, start=1):
        if p in dividers:
            prod *= i
    print(prod)

def cmp(a, b):
    x = right_order(a, b)
    return {
        True: -1,
        None: 0,
        False: 1,
        }[x]

def right_order(items_a, items_b, depth=0):
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
