# read_and_plot_tsv.py
# a simple Python script to read data from a Tab-Separated Variable (TSV) file 
# and plot the result

import    matplotlib.pyplot as plt
import csv
 
X = []
Y = []

# The example TSV input file is: 'RM_preemption_execution_data_out.txt'
 
with open('RM_example_data_s44_t3_out.txt', 'r') as datafile:
    plotting = csv.reader(datafile, delimiter='\t')
     
    for ROWS in plotting:
        X.append(float(ROWS[0]))
        Y.append(int(ROWS[1]))

# plot the result
plt.plot(X, Y)

# Feel free to make th elabels more personal
plt.title('Execution graph using CSV')
plt.xlabel('time in (us)')
plt.ylabel('Task-Number')

# The example Portable Network Graphics (PNG) file 
# for output is: 'RM_execution_graph.png'  
plt.savefig('RM_execution_graph.png', dpi=600)
# The number of "dots per inch" (dpi) can be adjusted

plt.show()