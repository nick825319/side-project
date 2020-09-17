# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

fdata = np.loadtxt('measure_cam_respond.txt', delimiter = ' ')

fdata = fdata * 1000 #to millisecond

# It is necessary to adjust the tick range of the y axis
# so that
#   (1) the highest tick is higher than the largest data value, and
#   (2) the smallest data value is visible

#ax.set_ylim(-3,50)
fig = plt.figure(figsize=(10,5))
ax = fig.add_subplot()


ax.set_xlabel('excution')
ax.set_ylabel('cam_respond(milli_sec)')
ax.set_title('Pi_camera_getImage_time')
average = sum(fdata) / len(fdata)
plt.yticks(np.arange(0,max(fdata)+average*0.1, 30))


plt.xticks(np.arange(0, len(fdata)+len(fdata)*0.01, len(fdata)/20), rotation=60)
ax.plot(fdata, linestyle='-')
ax.set_ylim(ymin=0,ymax=max(fdata)+average*0.1)
plt.savefig('./measure_cam_respond.pdf')
plt.show()

