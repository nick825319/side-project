# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker


fdata = np.loadtxt('measure_composer_trans_image.txt', delimiter = ' ')

# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,5))
ax = fig.add_subplot()


ax.set_xlabel('excution')
ax.set_ylabel('image_decode(miil_sec)')
ax.set_title('composer_transfer_image_time')

fdata = fdata*1000
average = sum(fdata) / len(fdata)
plt.yticks(np.arange(0,max(fdata)+average*0.1, average*0.05))
plt.xticks(np.arange(0, len(fdata)+len(fdata)*0.01, len(fdata)/20), rotation=60)
ax.plot(fdata, linestyle='-')
ax.set_ylim(ymin=0,ymax=max(fdata)+average*0.1)

plt.savefig('./measure_composer_trans_image.pdf')
plt.show()

