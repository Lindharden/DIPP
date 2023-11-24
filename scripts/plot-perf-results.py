import matplotlib.pyplot as plt
import pandas as pd

def plot_data(df, y, title, ylabel):
    plt.figure(figsize=(10, 6))
    plt.plot(df['Iteration'], df[y], marker='o')
    plt.title(title)
    plt.xlabel('Iteration')
    plt.ylabel(ylabel)
    plt.grid(True)
    plt.show()

def plot_cache_data(df):
    plt.figure(figsize=(10, 6))
    plt.plot(df['Iteration'], df['avg-cache-misses'], marker='o', label='Cache Misses')
    plt.plot(df['Iteration'], df['avg-cache-references'], marker='o', label='Cache References')
    plt.title('Cache Misses and References over Iterations')
    plt.xlabel('Iteration')
    plt.ylabel('Count')
    plt.legend()
    plt.grid(True)
    plt.show()

def main():
    csv_file = '../24-11-perf-benchmark.csv'
    df = pd.read_csv(csv_file)

    plot_data(df, 'avg-cpu-clock', 'CPU Clock over Iterations', 'CPU Clock')
    plot_data(df, 'avg-context-switches', 'Context Switches over Iterations', 'Context Switches')
    plot_cache_data(df)
    plot_data(df, 'avg-cache-miss-rate', 'Cache Miss Rate over Iterations', 'Cache Miss Rate (%)')
    plot_data(df, 'avg-peak-memory', 'Peak Memory over Iterations', 'Peak Memory (KB)')

if __name__ == "__main__":
    main()
