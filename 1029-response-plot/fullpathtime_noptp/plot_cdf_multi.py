import numpy as np
import matplotlib.pyplot as plt

str_cpu_utility = [ "ntp2", "ptp2"]

rt_list = []
n_list = []
rt_Sorted = []
i = 0
for x in str_cpu_utility:
    data_name = "measure_fullPathTime_byPi_" + str(x) + ".txt"

    rt_list.append(np.loadtxt(data_name, delimiter = ' '))
    n_tmp = np.arange(1, len(rt_list[i])+1) / np.float(len(rt_list[i]))*100
    n_list.append(n_tmp)

    rt_Sorted.append(np.sort(rt_list[i]*1000))

    i = i+1


fig = plt.figure(figsize=(8,6))
ax1 = fig.add_subplot()


#dir_name = "./70cpu-respT/"
#data_name = "measure_fullPathTime_byPi.txt"

#rt = np.loadtxt( dir_name + data_name, delimiter = ' ')
#rt2 = np.loadtxt( "./80cpu-respT/measure_fullPathTime_byPi.txt", delimiter = ' ')

#n2 = np.arange(1, len(rt2)+1) / np.float(len(rt2))*100
#millisecondRt2 = rt2*1000
#reSorted2 = np.sort(millisecondRt2)

#n = np.arange(1, len(rt)+1) / np.float(len(rt))*100
#millisecondRt = rt*1000
#reSorted = np.sort(millisecondRt)

i = 0
for x in rt_Sorted:
    ax1.step(x, n_list[i], label = str(str_cpu_utility[i]) + " cpu(%)")
    i = i+1

#ax1.step(reSorted, n, label = "rt")
#ax1.step(reSorted2, n2, label = "rt2")

ax1.set_xlabel("response Time (millisecond)")
ax1.set_ylabel("percentage (%)")

for x in rt_Sorted:
    print(f"index: {i} {max(x)}")

plt.xticks(np.arange(int(min(rt_Sorted[0]))-50, 500, 20), rotation=60)
plt.yticks(np.arange(0, 101, 10))

plt.legend(title="cpu utilization")
plt.tight_layout()
plt.savefig('./cdf_multi.pdf')
plt.show()

