import numpy as np
from typing import Union

def vector_norm(x : np.ndarray, p : Union[int, float, str] = 2) -> float:

    x = np.asarray(x).ravel()

    if len(x) == 0:
        return 0.0
    
    if p == 1:
        return np.sum(np.abs(x))
    elif p == 2:
        # scale by max to avoid overflow
        x_abs = np.abs(x)
        x_max = np.max(x_abs)
        if x_max == 0:
            return 0.0
        x_scaled = x_abs / x_max
        return x_max * np.sqrt(np.sum(x_scaled ** 2))
    elif p == np.inf:
        return np.max(np.abs(x))
    elif p == 0:
        return np.sum(x != 0)
    else:
        return np.sum(np.abs(x) ** p) ** (1.0 / p)
    

def matrix_norm(A : np.ndarray, ord : Union[int, str] = 'fro') -> float:

    A = np.asarray(A)

    if A.size == 0:
        return 0.0
    
    if ord == 'fro':
        return np.sqrt(np.sum(np.abs(A) ** 2))
    elif ord == 1:
        return np.max(np.sum(np.abs(A), axis=0))
    elif ord == 2:
        return np.linalg.svd(A, compute_uv=False)[0]
    elif ord == np.inf:
        return np.max(np.sum(np.abs(A), axis=1))
    else:
        raise ValueError(f"Unsupported matrix norm: {ord}")
    
def relative_error(x : np.ndarray, x_ref : np.ndarray, p : Union[int, str] = 2) -> float:
    x = np.asarray(x)
    x_ref = np.asarray(x_ref)
    
    if x.ndim == 1:
        norm_diff = vector_norm(x - x_ref, p)
        norm_ref = vector_norm(x_ref, p)
    else:
        norm_diff = matrix_norm(x - x_ref, p)
        norm_ref = matrix_norm(x_ref, p)
    
    if norm_ref == 0:
        return np.inf if norm_diff > 0 else 0.0
    
    return norm_diff / norm_ref

def relative_residual(A : np.ndarray, x : np.ndarray, b : np.ndarray) -> float:

    residual = A @ x - b
    return vector_norm(residual, 2) / vector_norm(b, 2)
