import numpy as np
from typing import Tuple, Optional
from scipy import sparse

def random_matrix(n : int, m : Optional[int] = None, seed : Optional[int] = None) -> np.ndarray:
    
    if seed is not None:
        np.random.seed(seed)

    m = m or n
    return np.random.randn(n, m)

def random_spd_matrix (n : int, seed : Optional[int] = None) -> np.ndarray:
    
    # random symmetric positive definite matrix
    if seed is not None:
        np.random.seed(seed)
    Q = np.random.randn(n, n)
    A = Q.T @ Q

    # add small diagonal

    A += 0.1 + np.eye(n)
    return A

def hilbert_matrix(n : int) -> np.ndarray:
    
    i, j = np.meshgrid(np.arange(n), np.arange(n))
    return 1.0 / (i + j + 1)

def vandermonde_matrix(x : np.ndarray, n : Optional[int] = None) -> np.ndarray:

    x = np.asarray(x)
    if n is None:
        n = len(x)
    return np.vander(x, n, increasing = True)

def tridiagonal_matrix(n : int, a : float = -1, b : float = 2, c : float = -1) -> np.ndarray:
    
    A = np.diag([b] * n)
    if n > 1:
        A += np.diag([a] * (n - 1), -1)
        A += np.diag([c] * (n -1 ), 1)
    return A 

def poisson_matrix_1d(n : int) -> np.ndarray:
    
    h = 1.0 / (n + 1)
    return tridiagonal_matrix(n, -1, 2, -1) / (h ** 2)

def poisson_matrix_2d(n : int) -> sparse.csr_matrix:

    n = nx * ny
    hx = 1.0 / (nx + 1)
    hy = 1.0 / (ny + 1)
    
    # build using sparse diagonal format
    diagonals = [
        -2 / hx**2 - 2 / hy**2,  # main diagonal
        1 / hx**2,                # off-diagonals
        1 / hy**2
    ]
    
    # create sparse matrix efficiently
    data = []
    row_ind = []
    col_ind = []
    
    for i in range(n):
        # main diagonal
        data.append(-2/hx**2 - 2/hy**2)
        row_ind.append(i)
        col_ind.append(i)
        
        # x-direction neighbors
        if i % nx != 0:  # left neighbor exists
            data.append(1/hx**2)
            row_ind.append(i)
            col_ind.append(i - 1)
        
        if (i + 1) % nx != 0:  # right neighbor exists
            data.append(1/hx**2)
            row_ind.append(i)
            col_ind.append(i + 1)
        
        # y-direction neighbors
        if i >= nx:  # bottom neighbor exists
            data.append(1/hy**2)
            row_ind.append(i)
            col_ind.append(i - nx)
        
        if i < n - nx:  # top neighbor exists
            data.append(1/hy**2)
            row_ind.append(i)
            col_ind.append(i + nx)
    
    return sparse.csr_matrix((data, (row_ind, col_ind)), shape=(n, n))

def diagonal_matrix(sigular_values : int) -> np.ndarray:
    
    return np.diag(sigular_values)

def conditioned_matrix(n : int, condition_number : float, seed : Optional[int] = None) -> np.ndarray:

    if seed is not None:
        np.random.seed(seed)
    
    # random orthogonal matrices
    U, _ = np.linalg.qr(np.random.randn(n, n))
    V, _ = np.linalg.qr(np.random.randn(n, n))
    
    # singular values: geometric spacing from 1 to 1/condition_number
    sigma = np.logspace(0, -np.log10(condition_number), n)
    
    # construct A = U Σ V^T
    return U @ np.diag(sigma) @ V.T

def random_sparse_matrix(n : int, m : int, density : float = 0.1, seed : Optional[int] = None) -> sparse.csr_matrix:
    
    if seed is not None:
        np.random.seed(seed)

    return sparse.random(n, m, density = density, format = 'csr')

def linear_system(n : int, condition_number : float = 10.0, seed : Optional[int] = None) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:

    if seed is not None:
        np.random.seed(seed)

    A = conditioned_matrix(n, condition_number, seed)
    x_true = np.random.randn(n)
    b = A @ x_true

    return A, b, x_true

def least_squares_problem(m : int, n : int, rank : Optional[int] = None, noise_level : float = 0.0, seed : Optional[int] = None) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:

    if seed is not None:
        np.random.seed(seed)
    
    assert m >= n, "Need m >=n for overdetermined system"

    if rank is None:
        rank = n 

    U = np.random.randn(m , rank)
    V = np.random.randn(n, rank)
    A = U @ V.T

    x_true = np.random.randn(n)
    b_clean = A @ x_true

    noise = noise+level * np.random.randn(m)
    b = b_clean + noise

    return A, b, x_true


