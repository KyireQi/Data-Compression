import numpy as np
import matplotlib.pyplot as plt

#xy = np.array([2.5, 2.4], [0.5, 0.7], [2.2, 2.9], [1.9, 2.2], [3.1, 3.0], [2.3, 2.7], [2, 1.6], [1, 1.1], [1.5, 1.6], [1.1, 0.9])
x = np.array([2.5, 0.5, 2.2, 1.9, 3.1, 2.3, 2, 1, 1.5, 1.1])
y = np.array([2.4, 0.7, 2.9, 2.2, 3.0, 2.7, 1.6, 1.1, 1.6, 0.9])

#均值归零
mean_x = np.mean(x)
mean_y = np.mean(y)
x_after = x - mean_x
y_after = y - mean_y
data = np.matrix([[x_after[i], y_after[i]] for i in range(len(x_after))])
plt.plot(x_after, y_after, 'o')

#求协方差矩阵
point_matrix = np.dot(np.transpose(data), data)
eig_val, eig_vec = np.linalg.eig(point_matrix)
# print(eig_vec)
plt.plot([eig_vec[:,0][0], 0], [eig_vec[:,0][1], 0], color = 'red')
plt.plot([eig_vec[:,1][0], 0], [eig_vec[:,1][1], 0], color = 'blue')

eig_pair = [(np.abs(eig_val[i]), eig_vec[:,i]) for i in range(len(eig_val))]
eig_pair.sort(reverse=True)
maxx = eig_pair[0][1]
# print(maxx)
new_data = np.dot(data, maxx)
print(new_data)
plt.show()