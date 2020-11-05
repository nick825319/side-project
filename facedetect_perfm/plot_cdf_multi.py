import numpy as np
import matplotlib.pyplot as plt

str_cpu_utility = ["cloud", "edge"]

rt_list = []
n_list = []
rt_Sorted = []
i = 0
for x in str_cpu_utility:
    dir_name = "./"
    data_name = "measure_facedetect_"+str(x)+"_filte.txt"

    rt_list.append(np.loadtxt( dir_name + data_name, delimiter = ' '))
    n_tmp = np.arange(1, len(rt_list[i])+1) / np.float(len(rt_list[i]))*100
    n_list.append(n_tmp)

    rt_Sorted.append(np.sort(rt_list[i]*1000))

    i = i+1


fig = plt.figure(figsize=(8,6))
ax1 = fig.add_subplot()

i = 0
for x in rt_Sorted:
    ax1.step(x, n_list[i], label = str(str_cpu_utility[i]))
    i = i+1

ax1.set_xlabel("response Time (millisecond)")
ax1.set_ylabel("percentage (%)")

for x in rt_Sorted:
    print(f"index: {i} {max(x)}")

plt.xticks(np.arange(0, max(rt_Sorted[1]), 10), rotation=60)
plt.yticks(np.arange(0, 101, 10))

plt.legend(title="type")
plt.tight_layout()
plt.savefig('./cdf_multi.pdf')
plt.show()

