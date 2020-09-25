import numpy as np
import matplotlib.pyplot as plt

rt = np.loadtxt("measure_toComposer_timestamp_time.txt", delimiter = ' ')

fig1, ax1 = plt.subplots();

n = np.arange(1, len(rt)+1) / np.float(len(rt))*100

millisecondRt = rt*1000
reSorted = np.sort(millisecondRt)





plt.xticks(np.arange(0, max(reSorted), max(reSorted)/20 ), rotation=60)

plt.boxplot(reSorted)

plt.savefig('./boxplot_measure_toComposer_timestamp_time.pdf')

plt.show()

