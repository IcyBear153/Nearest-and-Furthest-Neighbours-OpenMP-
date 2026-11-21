import sys
import matplotlib.pyplot as plt

files = sys.argv[1:]
fig, axes = plt.subplots(1, len(files), figsize=(5*len(files),4), squeeze=False, sharey=True)

for ax, filename in zip(axes[0], files):
    runs = {}
    with open(filename) as f:
        next(f)   # skip header
        for line in f:
            if line.strip():
                threads, seconds, schedule = line.strip().split(",",2)
                runs.setdefault(schedule, []).append((int(threads), float(seconds)))

    ones = [s for rows in runs.values() for t,s in rows if t==1]
    baseline = sum(ones)/len(ones)

    for schedule, rows in runs.items():
        threads = [t for t,s in rows]
        speedup = [baseline/s for t,s in rows]
        ax.plot(threads, speedup, "o-", label=schedule)

    most = max(threads)
    ax.plot([1,most], [1,most], "--", color="grey", label="ideal")
    ax.set(title=filename, xlabel="Number of threads", ylabel="Speed-up")
    ax.legend()

fig.savefig("schedules.png", dpi=200, bbox_inches="tight")
