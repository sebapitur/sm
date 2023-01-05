#!/usr/bin/python3
from matplotlib import pyplot as plt

# create figure and axis objects with subplots()
fig,ax = plt.subplots()
# make a plot

ax.plot([1, 2, 4, 8],
        [1.356601, 1.921705, 2.856401, 2.889225],
        color="red", 
        marker="o")
# set x-axis label
ax.set_xlabel("Procs", fontsize = 14)
# set y-axis label
ax.set_ylabel("Speedup",
              color="red",
              fontsize=14)


# twin object for two different y-axis on the sample plot
ax2=ax.twinx()
# make a plot with different y-axis using second axis object
ax2.plot([1, 2, 4, 8],
        [0.678301, 0.480426, 0.35705, 0.180577],
        color="blue",marker="o")
ax2.set_ylabel("Efficiency",color="blue",fontsize=14)
plt.show()
# save the plot as a file
fig.savefig('prim_omp_mpi_speedup.png',
            format='png',
            dpi=100,
            bbox_inches='tight')