"""
    IMS 2020/21 Project - Epidemic on macro level
    Data plotting
"""
import matplotlib.pyplot as plt
import pandas
import sys
import datetime
from dateutil import relativedelta
from matplotlib.dates import (DAILY, DateFormatter,
                              rrulewrapper, RRuleLocator, drange)

model = ""

def main(filename: str) -> None:
    """
    Reads file in CSV format and visualises the data it contains
    """

    # Read CSV file
    df = pandas.read_csv(filename)

    # Get data set size
    steps = df.index.stop
    discrete_steps = list(range(steps))

    S = df['S'] # Susceptible
    I = df['I'] # Infected
    R = df['R'] # Removed
    Isum = df['Isum'] # Total sum of infected

    # Plotting
    fig, ax = plt.subplots(figsize = (10,10))
    ax.set_xlim(0,len(discrete_steps))
    ax.set_ylim(0 - 1000000, S[0] + I[0] + R[0] + 1000000)

    ax.plot(discrete_steps, S, label='Susceptible', color='blue',   linewidth=1.5)
    ax.plot(discrete_steps, I, label='Infected',    color='green',  linewidth=1.5)
    ax.plot(discrete_steps, R, label='Recovered',   color='orange', linewidth=1.5)

    if model == "SEIRD":
        E = df['E'] # Exposed
        D = df['D'] # Dead
        ax.plot(discrete_steps, E, label='Exposed', color='yellow', linewidth=1.5)
        ax.plot(discrete_steps, D, label='Dead',    color='black',  linewidth=1.5)
        Rsum = df['Rsum'] # Total sum of recovered

    ax.set(xlabel='Time(days)', ylabel='Population count', title=model+" model simulation of COVID19 in Hubei")
    ax.ticklabel_format(style='plain')
    ax.legend()
    plt.show()

    if model == "SEIRD":
        dead_or_infected(S, Isum, Rsum, D, discrete_steps)
    else:
        fig, ax = plt.subplots(figsize = (10,10))
        ax.plot(discrete_steps, Isum, label='Infected',    color='green',  linewidth=1.5)
        ax.ticklabel_format(style='plain')
        ax.legend()
        plt.show()
    without_susceptible(E, I ,R, D, discrete_steps)


def dead_or_infected(S, Isum, Rsum, D, discrete_steps):
    # out = set_max(I, discrete_steps)
    fig, ax = plt.subplots(figsize = (10,10))

    ax.set_xlim(0,len(discrete_steps))

    if model == "SEIRD":
        ax.set_ylim(0, max(D) * 2 + max(Rsum))
        ax.plot(discrete_steps, D, label='Dead',    color='black',  linewidth=1.5)
    else:
         ax.set_ylim(0, max(Isum) + max(Rsum))

    ax.plot(discrete_steps, Isum, label='Infected',    color='green',  linewidth=1.5)
    ax.plot(discrete_steps, Rsum, label='Recovered',   color='orange', linewidth=1.5)

    ax.set(xlabel='Time(days)', ylabel='Population count', title=model+" model simulation of COVID19 in Hubei")
    ax.ticklabel_format(style='plain')
    ax.legend()
    plt.show()

def without_susceptible(E, I, R, D, discrete_steps):
    # Plotting
    fig, ax = plt.subplots(figsize = (10,10))
    ax.set_xlim(0,len(discrete_steps))
    ax.set_ylim(0,  max(I) + max(R))

    ax.plot(discrete_steps, I, label='Infected',    color='green',  linewidth=1.5)
    ax.plot(discrete_steps, R, label='Recovered',   color='orange', linewidth=1.5)

    if model == "SEIRD":
        ax.plot(discrete_steps, E, label='Exposed', color='yellow', linewidth=1.5)
        ax.plot(discrete_steps, D, label='Dead',    color='black',  linewidth=1.5)

    ax.set(xlabel='Time(days)', ylabel='Population count', title=model+" model simulation of COVID19 in Hubei")
    ax.ticklabel_format(style='plain')
    ax.legend()
    plt.show()



if __name__ == '__main__':
    model = sys.argv[1]
    if model == "SIR":
        main("statistics/data.csv")
    elif model == "SEIRD":
        main("statistics/data.csv")
    else:
        pass

