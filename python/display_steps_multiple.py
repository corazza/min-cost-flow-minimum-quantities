import sys, os
import matplotlib
import matplotlib.pyplot as plt
import json
from types import SimpleNamespace
import IPython
import random
import numpy as np
from os import walk

# DPI=600
# scale=1
# FIG_SIZE=(6.4*scale, 4.8*scale)
HORIZON=None # 0.3e5 # int(3e5)
PLOT_POINTS=250

exp_num = int(sys.argv[1])

results_dir = f'results/experiments_{exp_num}'

input_filenames = list()

for filename in os.listdir(results_dir):
    f = os.path.join(results_dir, filename)
    if os.path.isfile(f):
        input_filenames.append(f)

i = 0
for input_filename in input_filenames:
    
    with open(input_filename) as f:
        data = f.read()
        data = json.loads(data, object_hook=lambda d: SimpleNamespace(**d))
        xs = list(map(lambda a: a[0], data.steps))
        ys = list(map(lambda a: a[1], data.steps))

        p = data.generator_parameters.inclusion_p
        q = data.generator_parameters.max_span_q
        vlb_p = data.generator_parameters.vlb_p
        n_nodes = data.generator_parameters.n_nodes
        flow_value = data.generator_parameters.flow_value
        title = f'|V|={n_nodes}, F={flow_value}, p={p:.2f}, q={q}, vlb_p={vlb_p:.2f}'

        plt.figure()

        plt.ylabel("best cost")
        plt.xlabel("steps")
        plt.title(title)

        # plt.legend(loc="lower right")


        plt.plot(xs, ys, linewidth=1.0, color='blue', label='GA solver')
        plt.axhline(y=data.cplex_cost, color='r', linestyle='-', label='CPLEX')
        plt.legend(loc="lower right")
        
        plt.ylim(ymin=0)  # this line

        output_image_filename = f'{results_dir}/figure_{i+1}.png'
        i+=1
        plt.savefig(output_image_filename)
        print(f'saved to {output_image_filename}')

        # plt.show()
