# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

exp_path = './'

fdata = np.loadtxt(exp_path + 'composer_measure_fps.txt', delimiter = ' ')

# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,5))
ax = fig.add_subplot()

ax.set_xlabel('excution')
ax.set_ylabel('fps')
ax.set_title('composer_fps')

plt.xticks(np.arange(0, 430, 10), rotation=60)


ax.plot(fdata, linestyle='-')
#ax.xaxis.set_major_locator(x)
#plt.plot(fdata)




plt.savefig('./composer_measure_fps.pdf')
plt.show()

