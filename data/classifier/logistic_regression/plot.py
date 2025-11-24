import pandas as pd
import numpy as np
import itertools

from implementation import *

from skimage.color import rgb2lab

import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from mpl_toolkits.mplot3d import Axes3D

import xgboost as xgb

colors_df = pd.read_csv("../data_labeled_ambient.csv", usecols=["red", "green", "blue", "ambient", "label"])

X = colors_df[["red", "green", "blue", "ambient"]]

w = np.array([  0.54324152, -22.18843752,  49.2296672 , -34.39813628])
b = 1.7564837932259665

mean = X.mean(axis=0)
std = X.std(axis=0)

fig = plt.figure(figsize=(10, 8))
ax = fig.add_subplot(111, projection='3d')
idx = ['ambient', 'green', 'blue']
colors = {'ripe': 'red', 'unripe': 'yellow'}
for label in colors:
    subset = normalize(colors_df[colors_df['label'] == label],mean, std)
    ax.scatter(subset[idx[0]], subset[idx[1]], subset[idx[2]], c=colors[label], label=label,s=20)

# Create grid to plot the plane
x_range = np.linspace(subset[idx[0]].min(), subset[idx[0]].max(), 10)
y_range = np.linspace(subset[idx[1]].min(), subset[idx[1]].max(), 10)
x_grid, y_grid = np.meshgrid(x_range, y_range)
z_grid = -(w[3]*x_grid + w[1]*y_grid + b) / w[2]

# Plot the plane
ax.plot_surface(x_grid, y_grid, z_grid, alpha=0.3, color='cyan')


ax.set_xlabel(idx[0])
ax.set_ylabel(idx[1])
ax.set_zlabel(idx[2])
ax.legend()
plt.show()
