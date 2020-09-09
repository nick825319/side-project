# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


fdata = np.loadtxt('measure_C_imageIO.txt', delimiter = ' ')

# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,5))
ax = fig.add_subplot()


ax.set_xlabel('excution')
ax.set_ylabel('image_decode(sec)')
ax.set_title('composer_image_decode_time')

plt.yticks(np.arange(0,max(fdata)+0.01, 0.002))


plt.xticks(np.arange(0, 690, 30), rotation=60)
ax.plot(fdata, linestyle='-')
ax.set_ylim(ymin=0,ymax=max(fdata)+0.01)
plt.savefig('./measure_C_imageIO.pdf')
plt.show()

