import numpy as np

def average(data): 
    standardDeviation_sum=0

    sizeofdata = len(data)
    data = np.array(data)
    sumdata = sum(data)

    mean_values = sumdata/sizeofdata
    print(f"meanValue : {mean_values}")

    return mean_values

def write_avgToFile(avg_pair):
    save_path = "data_avg.txt"
    with open(save_path, 'w') as f:
        for s in avg_pair:
            f.write(str(s) + '\n')

if __name__ == "__main__":
    cpu_avg_pair = []
    str_cpu_utility = [50, 60, 70, 80, 90, 100]
    for x in str_cpu_utility:
        dir_name = ""
        data_name = "measure_detectObj_respTime_"+ str(x)+"cpu_filte.txt"
        data = np.loadtxt( dir_name + data_name, delimiter = ' ')

        data_average = average(data)

        cpu_avg_pair.append(str(x) + " " + str(data_average))
        
    write_avgToFile(cpu_avg_pair)


