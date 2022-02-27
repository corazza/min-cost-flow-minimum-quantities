import sys, os
import matplotlib
import matplotlib.pyplot as plt
import json
from types import SimpleNamespace
import IPython
import random
import numpy as np
from os import walk

exp_num = int(sys.argv[1])

results_output_dir = f'results/experiments_{exp_num}'
os.mkdir(results_output_dir)

f = open('tp0/tp0/generator_parameters.json')
original_generator_parameters = f.read()
generator_parameters = json.loads(original_generator_parameters)
f.close()
f = open('tp0/tp0/solver_parameters.json')
original_solver_parameters = f.read()
solver_parameters = json.loads(original_solver_parameters)
f.close()

steps = 8

inclusion_p_start = 0.3
inclusion_p_end = 0.9
inclusion_p_increment = (inclusion_p_end - inclusion_p_start) / (steps - 1)

vlb_p_start = 0.05
vlb_p_end = 0.5
vlb_p_increment = (vlb_p_end - vlb_p_start) / (steps - 1)

for i in range(0, steps):
    # generator_parameters['inclusion_p'] = inclusion_p_start + i * inclusion_p_increment
    generator_parameters['vlb_p'] = vlb_p_start + i * vlb_p_increment
    json_output_generator = json.dumps(generator_parameters, indent = 4)
    json_output_solver = json.dumps(solver_parameters, indent = 4)

    with open(os.getcwd() + '\TP0\TP0\generator_parameters.json', "w") as outfile:
        outfile.write(json_output_generator)

    with open(os.getcwd() + '\TP0\TP0\solver_parameters.json', "w") as outfile:
        outfile.write(json_output_solver)

    path = os.getcwd()
    os.chdir(path + '\\TP0\\TP0')
    os.system(path + '\\TP0\\x64\\Release\\TP0.exe')
    os.chdir(path)
    os.rename("tp0/tp0/results.json", results_output_dir + f'/results_{i+1}.json')

with open(os.getcwd() + '\TP0\TP0\generator_parameters.json', "w") as outfile:
    outfile.write(original_generator_parameters)

with open(os.getcwd() + '\TP0\TP0\solver_parameters.json', "w") as outfile:
    outfile.write(original_solver_parameters)
