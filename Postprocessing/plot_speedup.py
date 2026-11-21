import matplotlib.pyplot as plt
import numpy as np

threads, seconds = np.loadtxt("timings.csv", delimiter=",", skiprows=1, unpack=True)

plt.plot(threads, seconds[0] / seconds, "o-", label="measured")
plt.plot(threads, threads, "--", color="grey", label="ideal")
plt.xlabel("Number of threads")
plt.ylabel("Speed-up")
plt.title("Parallel speed-up")
plt.legend()
plt.savefig("speedup.png", dpi=200, bbox_inches="tight")