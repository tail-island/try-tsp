import sys

from random import Random

rng = Random()

n = int(sys.argv[1])

for i in range(n):
    print(f'{rng.uniform(-1, 1):.3f} {rng.uniform(-1, 1):.3f}')
