# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

fdata = np.loadtxt('measure_fullPathTime_byPi.txt', delimiter = ' ')

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
ax.set_title('Pi to jetson nano response time. With PTP')
average = sum(fdata) / len(fdata)
plt.yticks(np.arange(200,500, 20))


x_step = round((len(fdata)/20)/10)*10
plt.xticks(np.arange(0, len(fdata)+len(fdata)*0.01, x_step), rotation=60)
ax.plot(fdata, linestyle='-')
ax.set_ylim(ymin=200,ymax=500)
plt.savefig('./measure_fullpath_time_PTP.pdf')
plt.show()

