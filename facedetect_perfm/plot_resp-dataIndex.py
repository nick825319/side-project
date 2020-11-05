# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

fdata = np.loadtxt('measure_facedetect_edge_filte.txt', delimiter = ' ')

fdata = fdata * 1000 #to millisecond

# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,6))
ax = fig.add_subplot()


ax.set_xlabel('dataIndex')
ax.set_ylabel('full path time (millisecond)')
ax.set_title('face detect response time - edge')
average = sum(fdata) / len(fdata)
plt.yticks(np.arange(0,max(fdata)+20, 5))


x_step = round((len(fdata)/20)/10)*10
plt.xticks(np.arange(0, len(fdata)+len(fdata)*0.01, x_step), rotation=60)
ax.plot(fdata, linestyle='-')
ax.set_ylim(ymin=0,ymax=max(fdata)+20)
plt.savefig('./measure_facedetect_edge_filte.pdf')
plt.show()

