import numpy as np
import matplotlib.pyplot as plt

rt = np.loadtxt("measure_pi_composer_Jetson_repondTime.txt", delimiter = ' ')

fig1, ax1 = plt.subplots();

n = np.arange(1, len(rt)+1) / np.float(len(rt))*100

millisecondRt = rt*1000
reSorted = np.sort(millisecondRt)
ax1.step(reSorted, n)

ax1.set_xlabel("response Time (millisecond)")
ax1.set_ylabel("percentage (%)")

plt.xticks(np.arange(0, max(reSorted), max(reSorted)/20 ), rotation=60)

plt.tight_layout()
plt.savefig('./cdf_measure_pi_composer_Jetson_repondTime.pdf')
plt.show()

