import matplotlib.pyplot as plt
import numpy as np
import wave
import sys

f = input('file:\t')
spf = wave.open(f,'r')

signal = spf.readframes(-1)
signal = np.fromstring(signal, 'Int16')

plt.figure(1)
plt.title('Signal Wave...')
plt.plot(signal)
plt.show()
