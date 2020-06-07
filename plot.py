import matplotlib as mpl
import matplotlib.pyplot as plt
import math

def plot_line(filename, line_label):
    x_points = []
    x_label_points = []
    y_points = []

    read_file = open(filename, 'r')
    for line in read_file:
        x, y = line.split(',')
        x = int(x)
        y = float(y)


        x_points.append(math.log(x, 2))
        y_points.append(float(y))

        x_label = ""
        if (x < 1024):
            x_label = str(x) + "b"
        elif (x < (1024*1024)):
            x_label = str(x / 1024) + "kb"
        else:
            x_label = str(x / (1024*1024)) + "mb"

        x_label_points.append(x_label)
    

    plt.plot(x_points,y_points, label=line_label, linestyle='--', marker='o')
    plt.xticks(x_points, x_label_points, rotation=60)

plot_line("random.txt", "random access")
plot_line("linear.txt", "linear access")

plt.legend()
plt.xlabel("Size of array")
plt.ylabel("Avg access time, nanoseconds")
plt.tight_layout()

plt.savefig("benchmark.png")
