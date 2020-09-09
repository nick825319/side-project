# Read the documentation:
# https://matplotlib.org/3.1.1/api/_as_gen/matplotlib.pyplot.boxplot.html

# Note that we do not need to always use the seaborn library
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

fullpath_rasp = np.loadtxt('measure_pi_composer_Jetson_repondTime.txt', delimiter = ' ')
pi2C_transfer = np.loadtxt('measure_pi2C_transfer_time.txt', delimiter = ' ')
TransferLabel = np.loadtxt('measure_composer_Jetson_TransferLabelTime.txt', delimiter = ' ')
cam_respond = np.loadtxt('measure_cam_respond.txt', delimiter = ' ')
header = ['a','b','c','d']
#x = np.column_stack((fullpath_rasp,pi2C_transfer,TransferLabel,cam_respond))
a = np.array(fullpath_rasp)
print(a)
b= np.array(pi2C_transfer)
c= np.array(TransferLabel)
d= np.array(cam_respond)

x = np.concatenate((a,b,c,d), axis=0)
print(x)
X_AXIS = x[0]
print(x[0])

fig = plt.gcf()

configs = x[0]
N = len(configs)
ind = np.arange(N)
width = 0.1

p1 = plt.bar(ind, x[0], width, color='r')
p2 = plt.bar(ind, x[1], width, bottom=x[1] ,color='b')
p3 = plt.bar(ind, x[2], width, bottom=x[2],color='g')
p4 = plt.bar(ind, x[3], width, bottom=x[3],color='c')

plt.ylim([0,1])
plt.xticks(ind, X_AXIS, fontsize=12, rotation=90)
plt.legend((x[0],x[1],x[2],x[3]),(header[0],header[1],header[2],header[3]),fontsize=12, ncol=4, framealpha=0, fancybox=True)

#ax.hist(x, n_bins, density=True, histtype='bar', stacked=True)
#ax.set_title('respond time')
plt.show()

