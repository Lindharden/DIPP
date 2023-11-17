import pandas as pd
import matplotlib.pyplot as plt

# Load the data from the CSV file
data = pd.read_csv('../11-17-results.csv')

# Create a figure and a set of subplots
fig, ax = plt.subplots()

# Plot the data
ax.plot(data['module_num'], data['time_per_module'])

# Set the x and y axis labels
ax.set_xlabel('Amount of Modules')
ax.set_ylabel('Time per Module in MS')

# Set the x-ticks
ax.set_xticks(range(0, 100, 10))

# Add grid
ax.grid(True)

# Add a title
ax.set_title('Average execution time per module over 50 iterations')

# Show the plot
plt.show()
