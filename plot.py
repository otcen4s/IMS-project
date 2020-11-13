import matplotlib.pyplot as plt
import pandas


def plot_model(filename, model):
    df = pandas.read_csv(filename)

    #Population = df['Population']
    Susceptible = df['Susceptible']
    Infected = df['Infected']
    Recovered = df['Recovered']

    Exposed = Dead = ""

    if model == "SEIRD":
        Exposed = df['Exposed']
        Dead = df['Dead']

    steps = df.index.stop # ziska velkost celeho csv suboru (pocet krokov)

    discrete_steps = list(range(steps))

    fig, ax = plt.subplots()

    ax.plot(discrete_steps, Susceptible, label='susceptible', color='blue', linewidth=1.5)
    ax.plot(discrete_steps, Infected, label='infected', color='green', linewidth=1.5)
    ax.plot(discrete_steps, Recovered, label='recovered', color='orange', linewidth=1.5)
    
    title = "SIR"

    if model == "SEIRD":
        ax.plot(discrete_steps, Exposed, label='exposed', color='yellow', linewidth=1.5)
        ax.plot(discrete_steps, Dead, label='dead', color='black', linewidth=1.5)
        title = "SEIRD"

    ax.set(xlabel='Steps',
            ylabel='Population count',
            title=title)

    ax.legend()
    plt.show()


plot_model("data_SEIRD.csv", "SEIRD")
#plot_model("data_SIR.csv", "SIR")