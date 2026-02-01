import numpy as np
import time
from typing import Callable, List, Tuple, Dict


def count_dot_product_flops(n : int) -> int:
    
    multiplications = n
    additions = n - 1
    return multiplications + additions


def count_matvec_flops(m : int, n : int) -> int:
    
    # each row is a dot product of length n
    flops_per_row = 2 * n
    return m * flops_per_row


def count_matmul_flops(m : int, n : int, k : int) -> int:
    
    # result has m × n entries, each is dot product of length k
    flops_per_entry = 2 * k
    total_entries = m * n
    return flops_per_entry * total_entries


def count_triangular_solve_flops(n : int) -> int:
    
    # row i requires (i-1) mult/add pairs + 1 division
    total = 0
    for i in range(1, n + 1):
        total += 2 * (i - 1) + 1
    return total


def count_lu_flops(n : int) -> int:
    
    total = 0
    for k in range(n - 1):
        # column k: (n-k) divisions
        total += (n - k)
        # submatrix update: (n-k) × (n-k)
        total += 2 * (n - k) ** 2
    return total


def count_cholesky_flops(n : int) -> int:
   
    # approximately n³/3 (half of LU)
    return count_lu_flops(n) // 2


def count_qr_flops(m : int, n : int) -> int:
   
    # simplified estimate
    return 2 * m * n**2 - (2 * n**3) // 3


STANDARD_OPERATIONS = {
    'dot_product': count_dot_product_flops,
    'matvec': count_matvec_flops,
    'matmul': count_matmul_flops,
    'triangular_solve': count_triangular_solve_flops,
    'lu': count_lu_flops,
    'cholesky': count_cholesky_flops,
    'qr': count_qr_flops,
}


def flops_summary(n : int) -> Dict[str, int]:
    
    return {
        'dot_product': count_dot_product_flops(n),
        'matvec': count_matvec_flops(n, n),
        'matmul': count_matmul_flops(n, n, n),
        'triangular_solve': count_triangular_solve_flops(n),
        'lu': count_lu_flops(n),
        'cholesky': count_cholesky_flops(n),
        'qr': count_qr_flops(n, n),
    }

def estimate_complexity(func : Callable,
                       sizes : List[int],
                       setup_func : Callable,
                       n_runs : int = 3) -> Tuple[float, np.ndarray, np.ndarray]:
   
    sizes = np.array(sizes)
    times = np.zeros(len(sizes))
    
    for i, size in enumerate(sizes):
        # average over multiple runs
        run_times = []
        for _ in range(n_runs):
            problem = setup_func(size)
            
            start = time.perf_counter()
            func(*problem)
            elapsed = time.perf_counter() - start
            
            run_times.append(elapsed)
        
        times[i] = np.median(run_times)  # Use median for robustness
    
    # fit log(time) = log(c) + p*log(n)
    log_sizes = np.log(sizes)
    log_times = np.log(times)
    
    # linear regression
    A = np.vstack([np.ones_like(log_sizes), log_sizes]).T
    coeffs = np.linalg.lstsq(A, log_times, rcond=None)[0]
    
    exponent = coeffs[1]
    
    return exponent, sizes, times


def verify_complexity(func : Callable,
                     sizes : List[int],
                     setup_func : Callable,
                     expected_complexity : str,
                     tolerance : float = 0.3) -> bool:
    
    # parse expected complexity
    if 'n^3' in expected_complexity:
        expected_exp = 3.0
    elif 'n^2' in expected_complexity:
        expected_exp = 2.0
    elif 'n' in expected_complexity and '^' not in expected_complexity:
        expected_exp = 1.0
    else:
        raise ValueError(f"Cannot parse complexity: {expected_complexity}")
    
    # estimate empirical complexity
    actual_exp, _, _ = estimate_complexity(func, sizes, setup_func)
    
    # check if within tolerance
    deviation = abs(actual_exp - expected_exp)
    matches = deviation < tolerance
    
    print(f"Expected: {expected_exp}, Actual: {actual_exp:.2f}, "
          f"Deviation: {deviation:.2f}, Match: {matches}")
    
    return matches

def count_memory_bytes(dtype : str, shape : Tuple[int, ...]) -> int:
   
    bytes_per_element = {
        'float32': 4,
        'float64': 8,
        'int32': 4,
        'int64': 8,
    }
    
    if dtype not in bytes_per_element:
        raise ValueError(f"Unknown dtype: {dtype}")
    
    total_elements = np.prod(shape)
    return int(total_elements * bytes_per_element[dtype])


def memory_summary(n : int, dtype : str = 'float64') -> Dict[str, str]:
    
    def format_bytes(b):
        if b < 1024:
            return f"{b} B"
        elif b < 1024**2:
            return f"{b/1024:.2f} KB"
        elif b < 1024**3:
            return f"{b/1024**2:.2f} MB"
        else:
            return f"{b/1024**3:.2f} GB"
    
    return {
        'vector': format_bytes(count_memory_bytes(dtype, (n,))),
        'matrix': format_bytes(count_memory_bytes(dtype, (n, n))),
        'rank_k': format_bytes(count_memory_bytes(dtype, (n, n // 10))),
    }

def compute_gflops(flops : int, time_seconds : float) -> float:
    
    return flops / time_seconds / 1e9


def benchmark_operation(operation : str, n : int, n_runs : int = 10) -> Dict[str, float]:
    
    # generate problem
    if operation == 'dot_product':
        x = np.random.randn(n)
        y = np.random.randn(n)
        func = lambda: np.dot(x, y)
        flops = count_dot_product_flops(n)
        
    elif operation == 'matvec':
        A = np.random.randn(n, n)
        x = np.random.randn(n)
        func = lambda: A @ x
        flops = count_matvec_flops(n, n)
        
    elif operation == 'matmul':
        A = np.random.randn(n, n)
        B = np.random.randn(n, n)
        func = lambda: A @ B
        flops = count_matmul_flops(n, n, n)
        
    else:
        raise ValueError(f"Unknown operation: {operation}")
    
    # warm-up
    func()
    
    # time
    times = []
    for _ in range(n_runs):
        start = time.perf_counter()
        func()
        elapsed = time.perf_counter() - start
        times.append(elapsed)
    
    median_time = np.median(times)
    gflops = compute_gflops(flops, median_time)
    
    return {
        'operation': operation,
        'size': n,
        'flops': flops,
        'time_seconds': median_time,
        'gflops': gflops,
    }

class FlopCounter:
    
    def __init__(self):
        self.count = 0
    
    def __enter__(self):
        self.count = 0
        return self
    
    def __exit__(self, *args):
        pass
    
    def add(self):
        self.count += 1
    
    def multiply(self):
        self.count += 1
    
    def divide(self):
        self.count += 1
    
    def sqrt(self):
        self.count += 1
    
    def dot(self, n : int):
        self.count += 2 * n
    
    def matvec(self, m : int, n : int):
        self.count += count_matvec_flops(m, n)
