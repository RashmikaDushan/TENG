import numpy as np

# num_samples = 5 # data sample count
# time_steps = 10  # Number of time steps in each sample
# num_sensors = 6   # Number of sensors

# Initialize an empty list to store data for each channel
input_data = []

# Define the full file paths for your data files
sensor_train_files = [
    'sensor1_data.txt',
    'sensor2_data.txt',
    'sensor3_data.txt',
    'sensor4_data.txt',
    'sensor5_data.txt',
    'sensor6_data.txt'
]

# Load data from each text file and append it to channel_data
for filename in sensor_train_files:
    with open(filename, 'r') as file:
        lines = file.readlines()
        channel_values = [list(map(float, line.strip().split())) for line in lines]
        input_data.append(channel_values)

x = np.array(input_data).transpose(1, 2, 0)
print(x)