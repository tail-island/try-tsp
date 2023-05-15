import numpy as np
import sys

from funcy import filter, first
from math import dist
from neal import SimulatedAnnealingSampler
from operator import attrgetter
from pyqubo import Array, Constraint, Placeholder


BETA_RANGE = None
NUM_READS = 100
NUM_SWEEPS = 100_000


def solve(cities):
    n = len(cities)
    ds = np.array([[dist(city_0, city_1) for city_1 in cities] for city_0 in cities])

    ns = Array.create('ns', shape=(n, n), vartype='BINARY')
    k = Placeholder('k')

    h = (
        # https://proc-cpuinfo.fixstars.com/2020/01/tsp-sbm/

        1 / 2 * sum(ds[i, j] * ns[a, i] * (ns[(a + 1) % n, j] + ns[a - 1, j]) for a in range(n) for i in range(n) for j in range(n)) +
        k * Constraint(sum((sum(ns[a, i] for a in range(n)) - 1) ** 2 for i in range(n)), 'time') +
        k * Constraint(sum((sum(ns[a, i] for i in range(n)) - 1) ** 2 for a in range(n)), 'city')
    )

    model = h.compile()
    feed_dict = {'k': 2 / 3 * np.max(ds)}

    solution = min(model.decode_sampleset(SimulatedAnnealingSampler().sample(model.to_bqm(feed_dict=feed_dict), beta_range=BETA_RANGE, num_reads=NUM_READS, num_sweeps=NUM_SWEEPS),
                                          feed_dict=feed_dict),
                   key=attrgetter('energy'))

    for name, (ok, _) in solution.constraints().items():
        print(f"{name}: {'OK' if ok else 'NG'}", file=sys.stderr)

    if solution.constraints(only_broken=True):
        return []

    print(f"energy: {solution.energy}", file=sys.stderr)

    return tuple(map(lambda i: first(filter(lambda j: solution.array('ns', (i, j)), range(n))), range(n)))


if __name__ == '__main__':
    with open(sys.argv[1]) as f:
        cities = np.array([[float(string) for string in line.strip().split()] for line in f.readlines()])

    for node in solve(cities):
        print(node)
