import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt

# Your data
data = np.array([
])

# Sampling rate (1 / time between samples)
fs = 1 / 0.004  # 250 Hz

# Desired cutoff frequency of the filter, Hz
cutoff = 1.0  

# Design a low-pass filter
nyq = 0.5 * fs
normal_cutoff = cutoff / nyq
b, a = signal.butter(5, normal_cutoff, btype='low', analog=False)

# Apply the filter
filtered_data = signal.filtfilt(b, a, data)

# Plot the original and filtered data
plt.figure(figsize=(10, 6))
plt.plot(data, label='Original Data')
plt.plot(filtered_data, label='Filtered Data', linewidth=2)
plt.legend()
plt.title('Low-pass Filter')
plt.xlabel('Sample')
plt.ylabel('Amplitude')
plt.grid()
plt.show()
