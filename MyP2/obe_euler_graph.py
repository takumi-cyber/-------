import os
print(os.getcwd())
import matplotlib.pyplot as plt

# CSVファイルをpandasなしで読み込む
t = []
y = []
with open('data.data', 'r') as f:
    header = f.readline()  # ヘッダー行をスキップ
    for line in f:
        values = line.strip().split(',')
        t.append(float(values[0]))
        y.append(float(values[1]))

plt.plot(t, y, marker='o')
plt.xlabel('t軸')
plt.ylabel('y軸')
plt.title('ファイルから読み込んだグラフ')
plt.grid(True)
plt.show()




