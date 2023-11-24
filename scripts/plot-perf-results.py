import matplotlib.pyplot as plt
import pandas as pd

def plot_data(df, y, title, ylabel, color):
    plt.figure(figsize=(10, 6))
    plt.plot(df['Iteration'], df[y], color=color)
    plt.title(title)
    plt.xlabel('Amount of Modules')
    plt.ylabel(ylabel)
    # plt.yticks(range(900, 1500, 50)) # for peak memory
    plt.grid(True)
    plt.show()

def plot_cache_data(df):
    plt.figure(figsize=(10, 6))
    plt.plot(df['Iteration'], df['avg-cache-misses'], label='Cache Misses', color='red')
    plt.plot(df['Iteration'], df['avg-cache-references'], label='Cache References', color='blue')
    plt.title('Average Cache Misses and References with Increasing Modules')
    plt.xlabel('Amount of Modules')
    plt.ylabel('Count')
    plt.legend()
    plt.grid(True)
    plt.show()

def main():
    csv_file = '../24-11-perf-benchmark.csv'
    df = pd.read_csv(csv_file)

    # plot_data(df, 'avg-cpu-clock', 'Average CPU Clock with Increasing Modules', 'CPU Clock in msec', 'red')
    # plot_data(df, 'avg-context-switches', 'Average Context Switches with Increasing Modules', 'Amount of Context Switches', 'blue')
    # plot_cache_data(df)
    # plot_data(df, 'avg-cache-miss-rate', 'Average Cache Miss Rate with Increasing Modules', 'Cache Miss Rate (%)', 'orange')
    plot_data(df, 'avg-peak-memory', 'Average Peak Memory with Increasing Modules', 'Peak Memory (KB)', 'green')

if __name__ == "__main__":
    main()
