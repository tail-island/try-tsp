import sys

from math import cos, pi, sin

n = int(sys.argv[1])

for i in range(n):
    print(f'{cos(pi * 2 * i / n):.3f} {sin(pi * 2 * i / n):.3f}')
