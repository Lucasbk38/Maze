import matplotlib.pyplot as plt
import json

str = open("data.json", 'r')
data = json.load(str)

plt.plot(data)

plt.savefig("data.png")#, transparent = True)
