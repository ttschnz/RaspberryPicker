from typing import Tuple, List, Optional, Dict

import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

def normalize(X, mean, std):
    """Normalization of array
    Args:
       X (np.array): Dataset of shape (N, D)
       mean (np.array): Mean of shape (D, )
       std (float): Standard deviation of shape(D, )
    """
    return (X - mean) / std


def preprocess_data(
    df: pd.DataFrame,
    label: str,
    train_size: float = 0.6,
    seed: Optional[int] = None,
) -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, List[str], Dict[int, str],]:
    """Transforms data into numpy arrays and splits it into a train and test set

    Args:
        df: Data to split
        label: name of the training label
        train_size: proportion of the data used for training
        seed: random seed

    Returns:
        object: Tuple containing the training features, training label,
        test features, test label, names of the features and map from label to label_name
    """

    #df = df.sort_values(by=label)
    df[label] = df[label].astype("category")

    df = df.sample(frac=1, random_state=seed)
    train, test = (
        df[: int(len(df) * train_size)],
        df[int(len(df) * train_size) :],
    )

    X_train = train.drop(columns=label).to_numpy()
    X_test = test.drop(columns=label).to_numpy()

    y_train = train[label].cat.codes.to_numpy()
    y_test = test[label].cat.codes.to_numpy()

    label_map = dict(enumerate(df[label].cat.categories))
    feature_names = list(df.drop(columns=label).columns)

    return X_train, y_train, X_test, y_test, feature_names, label_map

def sigmoid(z: np.ndarray) -> np.ndarray:
    """ Sigmoid function
    
    Args:
        z: Input data of shape (N, )
        
    Returns:
        np.ndarray: Sigmoid of z of shape (N, ), where each value is in [0, 1]
    """
    s = 1 / (1 + np.exp(-z))
    return s


def logistic_output(X: np.ndarray, w: np.ndarray, b: float) -> np.ndarray:
    """ Output of logistic regression
    
    Args:
        X: Dataset of shape (N, d)
        w: Weights of logistic regression model of shape (d, )
        b: bias, a scalar
    Returns:
        p_hat (np.ndarray): Output of logistic regression of shape (N, )
    """
    p_hat = sigmoid(X @ w + np.ones(X.shape[0]) * b)
    return p_hat


def bce_loss(X: np.ndarray,  y: np.ndarray, w: np.ndarray, b: float) -> float:
    """ Binary cross-entropy loss function
    
    Args:
        X: Dataset of shape (N, d)
        y: Labels of shape (N, )
        w: Weights of logistic regression model of shape (d, )
        b: bias, a scalar
    
    Returns:
        float: binary cross-entropy loss.
    """
    epsilon = 1e-9
    
    N = X.shape[0]
    p_hat = logistic_output(X, w, b)
    loss = -(1/N) * np.sum(y * np.log(p_hat + epsilon) + (1 - y) * np.log(1 - p_hat + epsilon))
    
    return loss

def bce_gradient(X: np.ndarray, y: np.ndarray, w: np.ndarray, b: float) -> Tuple[np.ndarray, float]:
    """ Gradient of the binary-cross entropy loss
    
    Args:
        X: Dataset of shape (N, d)
        y: Labels of shape (N, )
        w: Weights of logistic regression model of shape (d, )
        b: bias, a scalar
        
    Returns:
        dw (np.ndarray) gradient of the loss with respect to w of shape (d, )
        db (float) gradient of the loss with respect to b, a scalar
    """
    N = X.shape[0]
    p_hat = logistic_output(X, w, b)
    dw = (1/N) * X.T @ (p_hat - y)
    db = (1/N) * sum(p_hat - y)
    
    return dw, db

def classify(p_hat: np.ndarray) -> np.ndarray:
    """ Classification function for binary class logistic regression. 
    
    Args:
        p_hat (np.array): Output of logistic regression of shape (N, )
    Returns:
        np.array: Label assignments of data of shape (N, )
    """
    y_hat = np.where(p_hat > 0.5, 1, 0)
    return y_hat

def accuracy(labels_gt: np.ndarray, labels_pred: np.ndarray) -> float:
    """Computes the accuracy.

    Args:
        labels_gt: labels (ground-truth) of shape (M, ).
        labels_pred: Predicted labels of shape (M, ).

    Returns:
        float: Accuracy, in range [0, 1].
    """
    return sum(labels_gt == labels_pred) / labels_gt.shape[0]

def train_logistic_regression(X: np.ndarray, 
                              y: np.ndarray, 
                              max_iters: int = 101, 
                              alpha: float = 0.5, 
                              loss_freq: int = 0) -> Tuple[np.ndarray, float, dict]:
    """ Training function for binary class logistic regression using gradient descent
    
    Args:
        X: Dataset of shape (N, d)
        y: Labels of shape (N, )
        max_iters: Maximum number of iterations. Default : 100
        alpha: The learning rate of  the gradient step. Default : 1
        loss_freq : Prints the loss every `loss_freq` iterations. Default : 0
        
    Returns:
        w: weights of shape (d, )
        b: scalar
        viz_d: dict used for visualizations
    """
    
    # Initialize weights
    np.random.seed(0)
    w = np.random.normal(0, 1, size=(X.shape[1], ))
    b = 0
    
    # Initialize dict with lists to keep track of loss, accuracy, weight and bias evolution
    logger = {'loss': [], 
             'acc': [], 
             'w': [],
             'b': []
            }
    
    
    for i in range(max_iters):
        # Compute loss, dw, db and update w and b 
        loss = bce_loss(X, y, w, b)
        dw, db = bce_gradient(X, y, w, b)
        
        w = w - alpha * dw
        b = b - alpha * db
        
        # Keep track of parameter, loss and accuracy values for each iteration
        logger['w'].append(w)
        logger['b'].append(b)
        logger['loss'].append(loss)
        p_hat = logistic_output(X, w, b)
        logger['acc'].append(accuracy(y, classify(p_hat)))
        
        if (loss_freq !=0) and i % loss_freq == 0:
            print(f'Loss at iter {i}: {loss:.5f}')
        
    if (loss_freq != 0):
        print('\nFinal loss: {:.5f}'.format(logger['loss'][-1]))
        
    return w, b, logger

def penalized_bce_loss(X: np.ndarray,  y: np.ndarray, w: np.ndarray, b: float, lambda_: float) -> float:
    """ Penalized binary cross-entropy loss function
    
    Args:
        X: Dataset of shape (N, d)
        y: Labels of shape (N, )
        w: Weights of logistic regression model of shape (d, )
        b: bias, a scalar
        lambda_: regularization coefficient (named this way as lambda is a reserved keyword in python)
    
    Returns:
        float: binary cross-entropy loss.
    """
    loss = bce_loss(X, y, w, b) + lambda_ * np.linalg.norm(w)**2
    return loss

def penalized_bce_gradient(X: np.ndarray,  
                           y: np.ndarray, 
                           w: np.ndarray, 
                           b: float, 
                           lambda_: float) -> Tuple[np.ndarray, float]:
    """ Gradient of the penalized binary-cross entropy loss
    
    Args:
        X: Dataset of shape (N, d)
        y: Labels of shape (N, )
        w: Weights of logistic regression model of shape (d, )
        b: bias, a scalar
        lambda_: regularization coefficient (named this way as lambda is a reserved keyword in python)
        
    Returns:
        dw (np.ndarray) gradient of the loss with respect to w of shape (d, )
        db (float) gradient of the loss with respect to b, a scalar
    """
    dw, db = bce_gradient(X, y, w, b)
    dw = dw + 2 * lambda_ * w
    db = db
    return dw, db


def train_penalized_logistic_regression(X: np.ndarray, 
                                        y: np.ndarray, 
                                        lambda_: float, 
                                        max_iters: int = 1001, 
                                        alpha: float = 0.5, 
                                        loss_freq: int = 0) -> Tuple[np.ndarray, float, dict]:
    """ Training function for binary class penalized logistic regression using gradient descent
    
    Args:
        X: Dataset of shape (N, d)
        y: Labels of shape (N, )
        lambda_: regularization coefficient (named this way as lambda is a reserved keyword in python)
        max_iters: Maximum number of iterations
        alpha: The learning rate of  the gradient step
        loss_freq : Prints the loss every `loss_freq` iterations
        
    Returns:
        w: weights of shape (d, )
        b: scalar
        viz_d: dict used for visualizations
    """
    
    # Initialize weights
    np.random.seed(0)
    w = np.random.normal(0, 1, size=(X.shape[1], ))
    b = 0
    
    # Initialize dict with lists to keep track of loss, accuracy, weight and bias evolution
    logger = {'loss': [], 
             'acc': [], 
            }
    
    
    for i in range(max_iters):
        # Compute loss, dw, db and update w and b 
        loss = penalized_bce_loss(X, y, w, b, lambda_)
        dw, db = penalized_bce_gradient(X, y, w, b, lambda_)
        
        w = w - alpha * dw
        b = b - alpha * db
        
        # Keep track of loss and accuracy values for each iteration
        logger['loss'].append(loss)
        
        p_hat = logistic_output(X, w, b)
        logger['acc'].append(accuracy(y, classify(p_hat)))
        
        if (loss_freq !=0) and i % loss_freq == 0:
            print(f'Loss at iter {i}: {loss:.5f}')
        
    if (loss_freq != 0):
        print('\nFinal loss: {:.5f}'.format(logger['loss'][-1]))
        
    return w, b, logger

def k_fold_indices(num_examples: int, k: int = 5) -> List[Tuple[np.ndarray, np.ndarray]]:
    """Generates indices for k-fold cross-validation

    Args:
        num_examples: Number of training examples
        k: Number of folds

    Returns:
        List of tuples containing the training indices and validation indices

    """
    indices = np.arange(num_examples)
    split_size = num_examples // k
    val_indices = [indices[k * split_size : (k + 1) * split_size] for k in range(k)]
    both_indices = [(np.delete(indices, val_ind), val_ind) for val_ind in val_indices]
    return both_indices


def cross_val_penalized_logistic_regression(X: np.ndarray,
                                            y: np.ndarray,
                                            lambda_: float = 0,
                                            max_iters: int = 1001,
                                            alpha: float = 0.5,
                                            loss_freq: int = 0,
                                            k: int = 5) -> float:
    """
    Performs k-fold cross-validation for penalized logistic regression and returns the mean validation accuracy
    
    For each fold, train the model and get the validation accuracy. Then, return the mean of all validation accuracies

    Args:
        X: Dataset of shape (N, d)
        y: Labels of shape (N, )
        lambda_: regularization coefficient (named this way as lambda is a reserved keyword in python)
        max_iters: Maximum number of iterations
        alpha: The learning rate of  the gradient step
        loss_freq : Prints the loss every `loss_freq` iterations
        k: Number of folds

    Returns:
        Mean validation accuracy

    """
    val_accs = []
    
    N = X.shape[0]
    for train_index, val_index in k_fold_indices(N, k):
        X_train = X[train_index]
        y_train = y[train_index]
        X_val = X[val_index]
        y_val = y[val_index]
        w, b, _ = train_penalized_logistic_regression(X_train, y_train, lambda_, max_iters, alpha, loss_freq)
        p_hat = classify(logistic_output(X_val, w, b))
        val_accs.append(accuracy(y_val, p_hat))

    return np.mean(val_accs)


def find_regularization_param(X_train, y_train, depth):
    lambdas = [0]+[10**-i for i in range(depth, -1, -1)]

    # cv_accs should contain the mean cross-validation accuracy for each value of lambda
    cv_accs = []

    for lambda_ in lambdas:
        cv_accs.append(
            cross_val_penalized_logistic_regression(X_train, y_train, lambda_=lambda_)
        )

        
    best_cv_acc = np.max(cv_accs)
    best_lambda = lambdas[np.argmax(cv_accs)]
    return best_cv_acc, best_lambda