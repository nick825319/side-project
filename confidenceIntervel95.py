import pandas as pd
import numpy as np

def confidenceintervel(data): 
    standardDeviation_sum=0

    sizeofdata = len(data)
    data = np.array(data)
    sumdata = sum(data)

    mean_values = sumdata/sizeofdata
    print(f"meanValue : {mean_values}")

    for index in data:
        standardDeviation_sum = standardDeviation_sum + (index-mean_values)**2

    standardDeviation_sum = standardDeviation_sum/sizeofdata
    standardDeviation_data = standardDeviation_sum**0.5
    print(f"standardDeviation_data : {standardDeviation_data}")

    LowerLimitingValue = mean_values - 1.654 * standardDeviation_data
    UpperLimitingValue = mean_values + 1.654 * standardDeviation_data
    return LowerLimitingValue, UpperLimitingValue


def filtedata(data, intervel):
    i = 0
    list_index = []
    print(f"intervel : {intervel}")
    for index in data:
        if index < intervel[0] or index > intervel[1]:
           list_index.append(i)   
        i += 1
    filted_data = np.delete(data, list_index)
    return filted_data 

def write_to_file(data):
    data_list = data.tolist()
    with open("measure_detectObject_respondse_Time_filted.txt", 'w') as f:
        for s in data_list:
            f.write(str(s) + '\n')

if __name__ == "__main__":

    data_name = "measure_detectObject_respondse_Time.txt"

    #data = pd.read_excel("measure_detectObject_respondse_Time.txt")
    data = np.loadtxt(data_name, delimiter = ' ')
    #print(data)
    #print("--------------")
    intervel = confidenceintervel(data)

    filted_data = filtedata(data, intervel)
    write_to_file(filted_data)
    



