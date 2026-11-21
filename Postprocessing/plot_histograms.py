import sys
import matplotlib.pyplot as plt
import numpy as np

label = sys.argv[1] if len(sys.argv) > 1 else "results"  # std or man

fig, (left, right) = plt.subplots(1, 2, figsize=(10, 4))

for ax, name in zip((left, right), ("nearest", "furthest")):
    ax.hist(np.loadtxt(f"{name}.csv"), bins=100)
    ax.set(title=f"Distance to {name} neighbour ({label})", xlabel="Distance", ylabel="Number of objects")

fig.savefig(f"histograms_{label}.png", dpi=200, bbox_inches="tight")
