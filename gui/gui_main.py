import os
import json

import tkinter as tk
from tkinter import ttk
from tkinter.messagebox import showinfo


class App(tk.Tk):
    def __init__(self):
        super().__init__()

        # Main interface setup
        self.title('Network traversal')
        self.geometry('800x600')
        ############################

        # Window label
        self.label = ttk.Label(self, text='Enter the required parameters\nand press Calculate to start the program', padding=20, font='Helvetica 18 bold', justify='center')
        self.label.pack()


        # Input setup

        self.label = ttk.Label(self, text='Alpha 1', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_1_entry = tk.Entry(self)
        self.alpha_1_entry.insert(0, 4)
        self.alpha_1_entry.pack()

        self.label = ttk.Label(self, text='Alpha 2', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_2_entry = tk.Entry(self)
        self.alpha_2_entry.insert(0, 5)
        self.alpha_2_entry.pack()

        self.label = ttk.Label(self, text='Alpha 3', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_3_entry = tk.Entry(self)
        self.alpha_3_entry.insert(0, 10)
        self.alpha_3_entry.pack()

        self.label = ttk.Label(self, text='Alpha 4', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.alpha_4_entry = tk.Entry(self)
        self.alpha_4_entry.insert(0, 10)
        self.alpha_4_entry.pack()

        self.label = ttk.Label(self, text='Max entry', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.cost_max_entry = tk.Entry(self)
        self.cost_max_entry.insert(0, 20)
        self.cost_max_entry.pack()

        self.label = ttk.Label(self, text='Flow value', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.flow_value_entry = tk.Entry(self)
        self.flow_value_entry.insert(0, 3046538816)
        self.flow_value_entry.pack()

        self.label = ttk.Label(self, text='Inclusion', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.inclusion_p_entry = tk.Entry(self)
        self.inclusion_p_entry.insert(0, 0.5)
        self.inclusion_p_entry.pack()

        self.label = ttk.Label(self, text='Max span', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.max_span_q_entry = tk.Entry(self)
        self.max_span_q_entry.insert(0, 7)
        self.max_span_q_entry.pack()

        self.label = ttk.Label(self, text='Number of nodes', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.n_nodes_entry = tk.Entry(self)
        self.n_nodes_entry.insert(0, 15)
        self.n_nodes_entry.pack()

        self.label = ttk.Label(self, text='Vlb', padding=2, font='Helvetica 8', justify='left')
        self.label.pack()
        self.vlb_p_entry = tk.Entry(self)
        self.vlb_p_entry.insert(0, 0.05000000074505806)
        self.vlb_p_entry.pack()

        # Calculate button
        self.button = ttk.Button(self, text='Calculate', padding=10)
        self.button['command'] = self.compile_cpp
        self.button.pack(anchor='se')

        # Display button
        self.button = ttk.Button(self, text='Display', padding=10)
        self.button['command'] = self.display_graph
        self.button.pack(anchor='se')

    def compile_cpp(self):
        self.output_parameters()
        path = os.getcwd()
        os.system('g++ ' + path + '\TP0\TP0\main.cpp -o main.exe && main.exe')

    def display_graph(self):
        path = os.getcwd()
        output_network_path = os.getcwd() + '\\TP0\\TP0\\output_network.json'
        output_solution_path = os.getcwd() + '\\TP0\\TP0\\output_solution.json'
        print('python ' + path + '\\python\\visualNetwork.py ' + output_network_path + ' ' + output_solution_path)
        os.system('python ' + path + '\\python\\visualNetwork.py ' + output_network_path + ' ' + output_solution_path)

    def output_parameters(self):

        output = {
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

        json_output = json.dumps(output, indent = 4)

        with open(os.getcwd() + '\TP0\TP0\generator_parameters.json', "w") as outfile:
            outfile.write(json_output)

        print('generator_parameters.json:\n' + json_output)


if __name__ == "__main__":
    app = App()
    app.mainloop()