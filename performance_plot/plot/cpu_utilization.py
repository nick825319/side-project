import seaborn as sns
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.lines as lines
import matplotlib.ticker as ticker
import scipy as sp
import scipy.stats

# this function is from https://stackoverflow.com/questions/15033511/compute-a-confidence-interval-from-sample-data
def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t.ppf((1+confidence)/2., n-1)
    return m, h

# References for appropriate color schemes:
# https://davidmathlogic.com/colorblind/
# https://colorbrewer2.org
majorColor='#dc267f'
minorColor='#648fff'
minorColor2='#ffb000'
majorColor2='#5e3c99'
#minorColor='#fdb863'
#majorColor='k'
#minorColor='r'
#minorColor2='b'

#import matplotlib as mpl
#print mpl.__version__

sns.set_style('ticks')
sns.set_context('paper',rc={'font.size':7,
                            'axes.titlesize':7,
                            'axes.labelsize':7,
                            'xtick.labelsize':7,
                            'ytick.labelsize':7,
                            'legend.fontsize':7})#,
#                            'lines.linewidth':2})
fig1 = plt.figure(figsize=(10,5))
ax1 = fig1.add_subplot(111)

exp_path = '../'

#s = ['EDF', 'RM', 'FIFO']
#m = ['o', 'x', 's']
#c = [majorColor, minorColor, minorColor2]
#l = ['-', '--', ':']
s = ['0', '1', '2','3']
m = ['d', 'x', 'o','^']
c = [minorColor2, minorColor, majorColor, majorColor2]
l = [':', '--', '-', '-.']


for x in range(0,4,1):
    nOut = []
    nOut_err = []
    for i in range(0,4):
        meta = np.loadtxt(exp_path + 'cpu_utilization_cutted' + '-' + str(i), dtype=float, delimiter = ' ')
        ax1.plot(meta, linestyle= l[i], color = c[i])
        #y, yerr = mean_confidence_interval(meta)
        #nOut.append(meta)
        #nOut_err.append(yerr)
    #ax1.errorbar([1.5, 4.5, 7.5, 10.5, 13.5], nOut, yerr=nOut_err, marker=m[x], color=c[x], lw=1.7, linestyle=l[x], capthick=0.7, capsize=2, mec=c[x], ms=4, mew=1, mfc='none')
	
sns.despine()
#plt.xlim(0,15)
#ax1.set_xticklabels(['1500', '4500', '7500', '10500', '13500'])
#ax1.set_xticks([1.5, 4.5, 7.5, 10.5, 13.5])
ax1.set_ylim(-5,105)

ax1.set_xlabel('time (second)')
ax1.set_ylabel('CPU utilization (%)')
ax1.xaxis.set_major_locator(ticker.MultipleLocator(2))
ax1.grid()

h_0 = lines.Line2D([],[],linewidth=1.7,linestyle='-',color=majorColor,marker='o', lw=0.7, mec=majorColor, ms=4, mew=1, mfc='none')
h_1 = lines.Line2D([],[],linewidth=1.7,linestyle='--',color=minorColor,marker='x', lw=0.7, mec=minorColor, ms=4, mew=1, mfc='none')
h_2 = lines.Line2D([],[],linewidth=1.7,linestyle=':',color=minorColor2,marker='d', lw=0.7, mec=minorColor2, ms=4, mew=1, mfc='none')
h_3 = lines.Line2D([],[],linewidth=1.7,linestyle='-.',color=majorColor2,marker='^', lw=0.7, mec=majorColor2, ms=4, mew=1, mfc='none')



plt.legend(('0','1','2','3'),loc=2)
#plt.tight_layout()
plt.show()
#plt.savefig('../../exp/exp-prioritization/exp2-utilization.pdf',
#            format='pdf', dpi=1000, bbox_inches='tight')
