from collections import namedtuple
import os
import json
import IPython

import tkinter as tk
from tkinter import ttk
from tkinter.messagebox import showinfo


class App(tk.Tk):
    def __init__(self):
        super().__init__()

        f = open('tp0/tp0/generator_parameters.json')
        generator_parameters = json.loads(f.read())
        f.close()
        f = open('tp0/tp0/solver_parameters.json')
        solver_parameters = json.loads(f.read())
        f.close()

        # Main interface setup
        self.title('Network traversal')
        self.geometry('1000x550')
        ############################

        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.columnconfigure(2, weight=1)

        # Window label
        self.label = ttk.Label(self, text='Enter the required parameters\nand press Calculate to start the program', padding=20, font='Helvetica 12 bold', justify='center')
        self.label.grid(column=1, row=0)

        # Input setup
        self.label = ttk.Label(self, text='Alpha 1', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=0, row=3)
        self.alpha_1_entry = tk.Entry(self)
        self.alpha_1_entry.insert(0, generator_parameters['alpha_1'])
        self.alpha_1_entry.grid(column=0, row=4)

        self.label = ttk.Label(self, text='Alpha 2', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=0, row=5)
        self.alpha_2_entry = tk.Entry(self)
        self.alpha_2_entry.insert(0, generator_parameters['alpha_2'])
        self.alpha_2_entry.grid(column=0, row=6)

        self.label = ttk.Label(self, text='Alpha 3', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=0, row=7)
        self.alpha_3_entry = tk.Entry(self)
        self.alpha_3_entry.insert(0, generator_parameters['alpha_3'])
        self.alpha_3_entry.grid(column=0, row=8)

        self.label = ttk.Label(self, text='Alpha 4', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=0, row=9)
        self.alpha_4_entry = tk.Entry(self)
        self.alpha_4_entry.insert(0, generator_parameters['alpha_4'])
        self.alpha_4_entry.grid(column=0, row=10)

        self.label = ttk.Label(self, text='Max entry', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=0, row=11)
        self.cost_max_entry = tk.Entry(self)
        self.cost_max_entry.insert(0, generator_parameters['cost_max'])
        self.cost_max_entry.grid(column=0, row=12)

        self.label = ttk.Label(self, text='Flow value', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=0, row=13)
        self.flow_value_entry = tk.Entry(self)
        self.flow_value_entry.insert(0, generator_parameters['flow_value'])
        self.flow_value_entry.grid(column=0, row=14)

        self.label = ttk.Label(self, text='Inclusion', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=0, row=15)
        self.inclusion_p_entry = tk.Entry(self)
        self.inclusion_p_entry.insert(0, generator_parameters['inclusion_p'])
        self.inclusion_p_entry.grid(column=0, row=16)

        self.label = ttk.Label(self, text='Max span', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=1, row=3)
        self.max_span_q_entry = tk.Entry(self)
        self.max_span_q_entry.insert(0, generator_parameters['max_span_q'])
        self.max_span_q_entry.grid(column=1, row=4)

        self.label = ttk.Label(self, text='Number of nodes', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=1, row=5)
        self.n_nodes_entry = tk.Entry(self)
        self.n_nodes_entry.insert(0, generator_parameters['n_nodes'])
        self.n_nodes_entry.grid(column=1, row=6)

        self.label = ttk.Label(self, text='Vlb', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=1, row=7)
        self.vlb_p_entry = tk.Entry(self)
        self.vlb_p_entry.insert(0, generator_parameters['vlb_p'])
        self.vlb_p_entry.grid(column=1, row=8)

        self.label = ttk.Label(self, text='Best of', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=1, row=9)
        self.best_of_entry = tk.Entry(self)
        self.best_of_entry.insert(0, solver_parameters['best_of'])
        self.best_of_entry.grid(column=1, row=10)

        self.label = ttk.Label(self, text='Gen. size', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=1, row=11)
        self.generation_size_entry = tk.Entry(self)
        self.generation_size_entry.insert(0, solver_parameters['generation_size'])
        self.generation_size_entry.grid(column=1, row=12)

        self.label = ttk.Label(self, text='No. pert.', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=1, row=13)
        self.num_perturbations_entry = tk.Entry(self)
        self.num_perturbations_entry.insert(0, solver_parameters['num_perturbations'])
        self.num_perturbations_entry.grid(column=1, row=14)

        self.label = ttk.Label(self, text='No. steps', padding=2, font='Helvetica 8', justify='left')
        self.label.grid(column=1, row=15)
        self.num_steps_entry = tk.Entry(self)
        self.num_steps_entry.insert(0, solver_parameters['num_steps'])
        self.num_steps_entry.grid(column=1, row=16)

        # Calculate button
        self.button = ttk.Button(self, text='Calculate', padding=10)
        self.button['command'] = self.compile_cpp
        self.button.grid(column=2, row=17)

        # Display button
        self.button = ttk.Button(self, text='Solution', padding=10)
        self.button['command'] = self.display_graph
        self.button.grid(column=2, row=18)

        self.button = ttk.Button(self, text='Results', padding=10)
        self.button['command'] = self.display_results
        self.button.grid(column=2, row=19)

    def compile_cpp(self):
        self.output_parameters()
        path = os.getcwd()
        # os.system('g++ ' + path + '\TP0\TP0\main.cpp -o main.exe && main.exe')
        os.chdir(path + '\\TP0\\TP0')
        os.system(path + '\\TP0\\x64\\Release\\TP0.exe')
        os.chdir(path)
        results_path = os.getcwd() + '\\TP0\\TP0\\results.json'
        with open(results_path) as f:
            data = f.read()
            data = json.loads(data)
            if len(data) == 0:
                tk.messagebox.showwarning(title='Warning', message='CPLEX nije nasao rjesenje, probajte opet')

    def display_graph(self):
        path = os.getcwd()
        output_network_path = os.getcwd() + '\\TP0\\TP0\\output_network.json'
        output_solution_path = os.getcwd() + '\\TP0\\TP0\\output_solution.json'
        # print('python ' + path + '\\python\\visualNetwork.py ' + output_network_path + ' ' + output_solution_path)
        os.system('python ' + path + '\\python\\visualNetwork.py ' + output_network_path + ' ' + output_solution_path)

    def display_results(self):
        path = os.getcwd()
        results_path = os.getcwd() + '\\TP0\\TP0\\results.json'
        # print('python ' + path + '\\python\\visualNetwork.py ' + output_network_path + ' ' + output_solution_path)
        os.system('python ' + path + '\\python\\display_steps.py ' + results_path)

    def output_parameters(self):
        output_generator = {
            "alpha_1": int(self.alpha_1_entry.get()),
            "alpha_3": int(self.alpha_3_entry.get()),
            "alpha_2": int(self.alpha_2_entry.get()),
            "alpha_4": int(self.alpha_4_entry.get()),
            "cost_max": int(self.cost_max_entry.get()),
            "flow_value": int(self.flow_value_entry.get()),
            "inclusion_p": float(self.inclusion_p_entry.get()),
            "max_span_q": int(self.max_span_q_entry.get()),
            "n_nodes": int(self.n_nodes_entry.get()),
            "vlb_p": float(self.vlb_p_entry.get())
        }

        output_solver = {
            "best_of": int(self.best_of_entry.get()),
            "generation_size": int(self.generation_size_entry.get()),
            "num_perturbations": int(self.num_perturbations_entry.get()),
            "num_steps": int(self.num_steps_entry.get()),
            "flow_value": int(self.flow_value_entry.get()),
            "improvements_stop": 1000
        }

        json_output_generator = json.dumps(output_generator, indent = 4)
        json_output_solver = json.dumps(output_solver, indent = 4)

        with open(os.getcwd() + '\TP0\TP0\generator_parameters.json', "w") as outfile:
            outfile.write(json_output_generator)

        with open(os.getcwd() + '\TP0\TP0\solver_parameters.json', "w") as outfile:
            outfile.write(json_output_solver)

        # print('generator_parameters.json:\n' + json_output)


if __name__ == "__main__":
    app = App()
    app.mainloop()
