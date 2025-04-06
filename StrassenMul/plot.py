import csv
import matplotlib.pyplot as plt

sizes = []
times = []

with open('performance.csv', newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        sizes.append(int(row['Matrix Size']))
        times.append(float(row['Time (seconds)']))

plt.figure(figsize=(10, 6))
plt.plot(sizes, times, marker='o', linestyle='-', color='royalblue')

plt.title('Strassen Matrix Multiplication Performance')
plt.xlabel('Matrix Size (N x N)')
plt.ylabel('Time (seconds)')
plt.grid(True)
plt.xscale('log', base=2)  
plt.yscale('linear')          
plt.xticks(sizes, labels=[str(size) for size in sizes], rotation=45)
plt.tight_layout()

plt.savefig("strassen_performance.png")
plt.show()
