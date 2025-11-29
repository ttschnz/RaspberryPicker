from typing import Tuple, List, Optional

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

from implementation import *

# Load data
colors_df = pd.read_csv('../data_labeled_ambient_width.csv',  usecols=["red", "green", "blue", "ambient", "width", "label"])

# # preprocess: don't train on ambient light, but use it to de-noise the color channels
# colors_df["red"] = colors_df["red"] - colors_df["ambient"]
# colors_df["green"] = colors_df["green"] - colors_df["ambient"]
# colors_df["blue"] = colors_df["blue"] - colors_df["ambient"]

# colors_df=colors_df.drop(columns=["ambient"])

# split into train and test dataset
X_train, y_train, X_test, y_test, feature_names, label_map = preprocess_data(colors_df, label="label", train_size=0.70, seed=12)

# Normalize data
mean = X_train.mean(axis=0)
std = X_train.std(axis=0)

X_train = normalize(X_train, mean, std)
X_test = normalize(X_test, mean, std)


# Train with "normal" logistic regression
w, b, _ = train_logistic_regression(X_train, y_train, max_iters=500_001, alpha=0.5, loss_freq=50000)

# evaluate results
print("------------------")
p_hat = logistic_output(X_train, w, b)
acc = accuracy(y_train, classify(p_hat))
print(f'Training accuracy: {100 * acc:.2f}%')

p_hat = logistic_output(X_test, w, b)
acc = accuracy(y_test, classify(p_hat))
print(f'Testing accuracy: {100 * acc:.2f}%')

p_hat = fast_logistic_output(X_test, w, b)
acc = accuracy(y_test, classify(p_hat))
print(f'Testing accuracy with fast_sigmoid: {100 * acc:.2f}%')
print("------------------")





# # Train with penalized logistic regression
# best_cv_acc, best_lambda = find_regularization_param(X_train, y_train, 10)
# print(f"Best lambda: {best_lambda}")
# print(f"Best CV acc: {best_cv_acc:.3f}")
# w, b, _ = train_penalized_logistic_regression(X_train, y_train, lambda_=best_lambda, max_iters=500_001, alpha=0.5, loss_freq=50000)

# # evaluate results
# print("------------------")
# p_hat = logistic_output(X_train, w, b)
# acc = accuracy(y_train, classify(p_hat))
# print(f'Training accuracy: {100 * acc:.2f}%')

# p_hat = logistic_output(X_test, w, b)
# acc = accuracy(y_test, classify(p_hat))
# print(f'Testing accuracy: {100 * acc:.2f}%')
# print("------------------")


def print_cpp_definitions(params, name_prefix="logistic_regression"):
    import numpy as np

    # Helper function to format numpy arrays into C++ initializer lists
    def format_array(arr):
        return ", ".join(f"{x:.8f}" for x in arr)

    w = params["w"]
    b = params["b"]
    mean = params["mean"]
    std = params["std"]

    print(f"const double {name_prefix}_w[{len(w)}] = {{{format_array(w)}}};")
    print(f"const double {name_prefix}_b = {float(b):.16f};")
    print(f"const double {name_prefix}_mean[{len(mean)}] = {{{format_array(mean)}}};")
    print(f"const double {name_prefix}_std[{len(std)}] = {{{format_array(std)}}};")


print_cpp_definitions({"w":w, "b": b, "mean": mean, "std": std}, name_prefix="ColorSensor::logistic_regression")