# SPEED-UP against number of threads, one line per loop.
# Each CSV comes from powershell timing loop (threads, seconds, schedule)

import sys
import matplotlib.pyplot as plt

files = sys.argv[1:]
# one panel per file # 'squeeze = False' keeps grid and 'sharey = True' gives evry panel same y axis for comparison
fig, axes = plt.subplots(1, len(files), figsize=(5*len(files),4), squeeze=False, sharey=True)

for ax, filename in zip(axes[0], files):
    runs = {} # for each schedule - (threads,seconds)
    with open(filename) as f:
        next(f)   # skip header
        for line in f:
            if line.strip():
                # split at only the first 2 commas and the schedule goes last because 'static,1' has a comma init
                threads, seconds, schedule = line.strip().split(",",2)
                runs.setdefault(schedule, []).append((int(threads), float(seconds))) # gives empty list the first time shedule appears

    # with 1 thread schdule makes 0 difference.
    # So all  single thread times are aveged onto the baseline whic evens out wobble
    ones = [s for rows in runs.values() for t,s in rows if t==1]
    baseline = sum(ones)/len(ones)

    for schedule, rows in runs.items():
        threads = [t for t,s in rows]
        # speed-up - how many times faster than 1 thread
        speedup = [baseline/s for t,s in rows]
        ax.plot(threads, speedup, "o-", label=schedule)

    most = max(threads)
    ax.plot([1,most], [1,most], "--", color="grey", label="ideal")   # n threads would be ideal as its n times faster
    ax.set(title=filename, xlabel="Number of threads", ylabel="Speed-up")
    ax.legend()

# Saves schedules.png
fig.savefig("schedules.png", dpi=200, bbox_inches="tight")
