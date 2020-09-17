# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

fdata = np.loadtxt('measure_composer_Jetson_TransferLabelTime.txt', delimiter = ' ')

fdata = fdata*1000
# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,5))
ax = fig.add_subplot()

ax.set_xlabel('excution')
ax.set_ylabel('TransferTime(milli_sec)')
ax.set_title('composer_Jetson_TransferLabelTime')
average = sum(fdata) / len(fdata)
plt.yticks(np.arange(0,1.5, 0.05))


plt.xticks(np.arange(0, len(fdata)+len(fdata)*0.01, round(len(fdata)/20)), rotation=60)
ax.plot(fdata, linestyle='-')
ax.set_ylim(ymin=0,ymax=1.5)
plt.savefig('./measure_composer_Jetson_TransferLabelTime.pdf')
plt.show()


