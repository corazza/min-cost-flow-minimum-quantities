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
        self.geometry('800x1000')
        ############################

        # Window label
        self.label = ttk.Label(self, text='Enter the required parameters\nand press Calculate to start the program', padding=20, font='Helvetica 18 bold', justify='center')
        self.label.pack()

        # Input setup
        self.label = ttk.Label(self, text='Alpha 1', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_1_entry = tk.Entry(self)
        self.alpha_1_entry.insert(0, generator_parameters['alpha_1'])
        self.alpha_1_entry.pack()

        self.label = ttk.Label(self, text='Alpha 2', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_2_entry = tk.Entry(self)
        self.alpha_2_entry.insert(0, generator_parameters['alpha_2'])
        self.alpha_2_entry.pack()

        self.label = ttk.Label(self, text='Alpha 3', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_3_entry = tk.Entry(self)
        self.alpha_3_entry.insert(0, generator_parameters['alpha_3'])
        self.alpha_3_entry.pack()

        self.label = ttk.Label(self, text='Alpha 4', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_4_entry = tk.Entry(self)
        self.alpha_4_entry.insert(0, generator_parameters['alpha_4'])
        self.alpha_4_entry.pack()

        self.label = ttk.Label(self, text='Max entry', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.cost_max_entry = tk.Entry(self)
        self.cost_max_entry.insert(0, generator_parameters['cost_max'])
        self.cost_max_entry.pack()

        self.label = ttk.Label(self, text='Flow value', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.flow_value_entry = tk.Entry(self)
        self.flow_value_entry.insert(0, generator_parameters['flow_value'])
        self.flow_value_entry.pack()

        self.label = ttk.Label(self, text='Inclusion', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.inclusion_p_entry = tk.Entry(self)
        self.inclusion_p_entry.insert(0, generator_parameters['inclusion_p'])
        self.inclusion_p_entry.pack()

        self.label = ttk.Label(self, text='Max span', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.max_span_q_entry = tk.Entry(self)
        self.max_span_q_entry.insert(0, generator_parameters['max_span_q'])
        self.max_span_q_entry.pack()

        self.label = ttk.Label(self, text='Number of nodes', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.n_nodes_entry = tk.Entry(self)
        self.n_nodes_entry.insert(0, generator_parameters['n_nodes'])
        self.n_nodes_entry.pack()

        self.label = ttk.Label(self, text='Vlb', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.vlb_p_entry = tk.Entry(self)
        self.vlb_p_entry.insert(0, generator_parameters['vlb_p'])
        self.vlb_p_entry.pack()

        self.label = ttk.Label(self, text='Best of', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.best_of_entry = tk.Entry(self)
        self.best_of_entry.insert(0, solver_parameters['best_of'])
        self.best_of_entry.pack()

        self.label = ttk.Label(self, text='Gen. size', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.generation_size_entry = tk.Entry(self)
        self.generation_size_entry.insert(0, solver_parameters['generation_size'])
        self.generation_size_entry.pack()

        self.label = ttk.Label(self, text='No. pert.', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.num_perturbations_entry = tk.Entry(self)
        self.num_perturbations_entry.insert(0, solver_parameters['num_perturbations'])
        self.num_perturbations_entry.pack()

        self.label = ttk.Label(self, text='No. steps', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.num_steps_entry = tk.Entry(self)
        self.num_steps_entry.insert(0, solver_parameters['num_steps'])
        self.num_steps_entry.pack()

        # Calculate button
        self.button = ttk.Button(self, text='Calculate', padding=10)
        self.button['command'] = self.compile_cpp
        self.button.pack(anchor='se')

        # Display button
        self.button = ttk.Button(self, text='Solution', padding=10)
        self.button['command'] = self.display_graph
        self.button.pack(anchor='se')

        self.button = ttk.Button(self, text='Results', padding=10)
        self.button['command'] = self.display_results
        self.button.pack(anchor='se')

    def compile_cpp(self):
        self.output_parameters()
        path = os.getcwd()
        # os.system('g++ ' + path + '\TP0\TP0\main.cpp -o main.exe && main.exe')
        os.chdir(path + '\\TP0\\TP0')
        os.system(path + '\\TP0\\x64\\Release\\TP0.exe')
        os.chdir(path)

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