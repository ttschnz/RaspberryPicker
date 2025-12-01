import pandas as pd

import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt


colors_df = pd.read_csv("../data_labeled_ambient_width.csv")

X = colors_df[["red","green","blue","ambient","width"]].values
C = X.T@X
eigres = np.linalg.eig(C)

sum(eigres.eigenvalues[0:4])/sum(eigres.eigenvalues)
sum(eigres.eigenvalues[0:3])/sum(eigres.eigenvalues)
sum(eigres.eigenvalues[0:2])/sum(eigres.eigenvalues)
sum(eigres.eigenvalues[0:1])/sum(eigres.eigenvalues)

# we chose 1 for 98.96% accuracy
theta_ast = eigres.eigenvectors[0:1].T
A=X@theta_ast
labels = colors_df["label"]
print(theta_ast)
## plotting

# Separate values by label
unripe_values = A[labels == 'unripe']
ripe_values = A[labels == 'ripe']
print(unripe_values)
# Create scatter plot
plt.figure(figsize=(10, 6))
plt.scatter(range(len(unripe_values)),unripe_values, color='orange', label='Unripe')
plt.scatter(range(len(ripe_values)),ripe_values, color='green', label='Ripe')

plt.xlabel('Sample Index')
plt.ylabel('Value')
plt.title('Scatter plot of Values by Ripeness')
plt.legend()
plt.show()



