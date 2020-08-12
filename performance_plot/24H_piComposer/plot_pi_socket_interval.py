# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

exp_path = './pi/'

fdata = np.loadtxt(exp_path + 'measure_socket_interval.txt', delimiter = ' ')

# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,5))
ax = fig.add_subplot()

ax.set_xlabel('hour')
ax.set_ylabel('socket_interval(sec)')
ax.set_title('pi each socket transfer interval')

plt.yticks(np.arange(round(min(fdata),4), round(max(fdata),4), 0.001))

labels = range(0,26)
ax.set_xticklabels(labels)

plt.xticks(np.arange(0, 182122, 7284), rotation=60)

ax.plot(fdata, linestyle='-')

plt.savefig('./pi_socket_interval.pdf')
plt.show()

