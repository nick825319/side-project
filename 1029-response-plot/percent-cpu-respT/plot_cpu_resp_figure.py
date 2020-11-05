import numpy as np
import matplotlib.pyplot as plt

data = np.loadtxt('data_avg.txt')

x = data[:, 0]
y = data[:, 1]
y = y*1000

fig = plt.figure(figsize=(10,6))
ax1 = fig.add_subplot()



ax1.plot(x, y, 'bo-')

ax1.set_xlabel("cpu utilization (%)")
ax1.set_ylabel("response time (millisecond)")
ax1.set_title("response time of cpu utilization")

#plt.xticks(np.arange(0, len(fdata)+len(fdata)*0.01, x_step), rotation=60)

plt.yticks(np.arange(0,400, 20))

plt.savefig('./cpu_response_figure.pdf')
plt.show()
