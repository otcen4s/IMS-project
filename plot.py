import csv
import matplotlib.pyplot as plt
import numpy as np
import pandas
from ipywidgets import interact
from matplotlib import figure

df = pandas.read_csv('data.csv')

Population = df['Population']
Susceptible = df['Susceptible']
Infected = df['Infected']
Recovered = df['Recovered']

steps = df.index.stop # ziska velkost celeho csv suboru (pocet krokov)

discrete_steps = list(range(steps))


fig, ax = plt.subplots()


ax.set(xlabel='Steps',
       ylabel='Population count',
       title='SIR')


ax.plot(discrete_steps, Susceptible, label='susceptible', color='blue', linewidth=1.5)
ax.plot(discrete_steps, Infected, label='infected', color='green', linewidth=1.5)
ax.plot(discrete_steps, Recovered, label='recovered', color='orange', linewidth=1.5)

ax.legend()
plt.show()