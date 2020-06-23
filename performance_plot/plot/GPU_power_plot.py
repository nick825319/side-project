# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

exp_path = '../'

fdata = np.loadtxt(exp_path + 'GPU_comsumption', dtype=int, delimiter = ' ')

# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,5))
ax = fig.add_subplot(111)

ax.set_xlabel('time (second)')
ax.set_ylabel('mW')
#ax.set_title('power compution')
y = range(fdata.size)
x = fdata
xyarray = np.array((y,x)).T

ax.plot(fdata)
ax.xaxis.set_major_locator(ticker.MultipleLocator(3))
#plt.plot(fdata)
count = 0
for x in fdata:
 if count%4 == 0 and count < fdata.size:
  ax.annotate('(%s)'% x, (xyarray[count]), textcoords='data')
 count += 1

plt.grid()
plt.savefig('./plot_GPU.pdf')
plt.show()

