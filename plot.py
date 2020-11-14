"""
    IMS 2020/21 Project - Epidemic on macro level
    Data plotting
"""
import matplotlib.pyplot as plt
import pandas
import sys

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

    # Plotting
    fig, ax = plt.subplots()
    ax.plot(discrete_steps, S, label='Susceptible', color='blue',   linewidth=1.5)
    ax.plot(discrete_steps, I, label='Infected',    color='green',  linewidth=1.5)
    ax.plot(discrete_steps, R, label='Recovered',   color='orange', linewidth=1.5)

    if model == "SEIRD":
        E = df['E'] # Exposed
        D = df['D'] # Dead
        ax.plot(discrete_steps, E, label='Exposed', color='yellow', linewidth=1.5)
        ax.plot(discrete_steps, D, label='Dead',    color='black',  linewidth=1.5)

    ax.set(xlabel='Steps', ylabel='Population count', title=model)
    ax.legend()
    plt.show()

if __name__ == '__main__':
    model = sys.argv[1]
    if model == "SIR":
        main("statistics/data_SIR.csv")
    elif model == "SEIRD":
        main("statistics/data_SEIRD.csv")
    else:
        pass

