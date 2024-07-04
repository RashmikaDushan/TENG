import numpy as np
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv1D, MaxPooling1D, Dropout, Flatten, Dense
from sklearn.model_selection import train_test_split
from tensorflow.keras.utils import to_categorical

num_samples = 126 # data sample count
time_steps = 200  # Number of time steps in each sample
num_sensors = 6   # Number of sensors

# Initialize an empty list to store data for each channel
input_data = []
input_labels = []

# Define the full file paths for your files
data_files = [
    'sensor1_data.txt',
    'sensor2_data.txt',
    'sensor3_data.txt',
    'sensor4_data.txt',
    'sensor5_data.txt',
    'sensor6_data.txt'
]

label_file = "label_data.txt"

# Load data from each text file and append it to channel_data
for filename in data_files:
    with open(filename, 'r') as file:
        lines = file.readlines()
        channel_values = [list(map(float, line.strip().split())) for line in lines]
        input_data.append(channel_values)

x = np.array(input_data).transpose(1, 2, 0)

# Load labels from the label file
with open(label_file, 'r') as file:
    lines = file.readlines()
    input_labels = [int(value) for line in lines for value in line.strip().split()]

y_labels = np.array(input_labels)


y = to_categorical(y_labels)  # One-hot encode the numerical labels

# print(np.shape(x))
# print(np.shape(y))

# Split the data into training and test sets
x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.2, random_state=42)

# Building the model
model = Sequential()
model.add(Conv1D(filters=64, kernel_size=3, activation='relu', input_shape=(time_steps, num_sensors)))
model.add(MaxPooling1D(pool_size=2))
model.add(Dropout(0.5))

model.add(Conv1D(filters=128, kernel_size=3, activation='relu'))
model.add(MaxPooling1D(pool_size=2))
model.add(Dropout(0.5))

model.add(Flatten())
model.add(Dense(256, activation='relu'))
model.add(Dropout(0.5))
model.add(Dense(2, activation='softmax'))  # Assuming 10 different people

# Compiling the model
model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

# Training the model
model.fit(x_train, y_train, epochs=100, batch_size=32, validation_split=0.2)

# Evaluating the model on the test data
loss, accuracy = model.evaluate(x_test, y_test)
print(f'Test Loss: {loss:.4f}, Test Accuracy: {accuracy:.4f}')

