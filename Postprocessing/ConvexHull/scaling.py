# // Andrew's monotone chain convex hull algorithm 

import re
import subprocess
from pathlib import Path
import matplotlib.pyplot as plt

prog = Path("build/nearest_furthest").with_suffix(".exe" if Path().resolve().drive else "")
sizes = {   "naive": [5000, 10000, 20000, 40000, 80000],
            "half" : [5000, 10000, 20000, 40000, 80000],
            "fast" : [20000, 50000, 100000, 200000, 500000, 1000000],
}

def bench(algo, n):
    out = subprocess.check_output([prog, "-n", str(n), "-d", "std", "-p", "-a", algo], text=True)
    t = float(re.search(r"Time taken:\s*([\d.]+)", out).group(1))
    print(f"{algo}, {n} points: {t} s")
    return t

for algo, counts in sizes.items():
    plt.loglog(counts, [bench(algo, n) for n in counts], "o-", label=algo)

plt.xlabel("Number of points")
plt.ylabel("Time taken (s)")
plt.legend()
plt.savefig("scaling.png", dpi=200, bbox_inches="tight")