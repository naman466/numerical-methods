import time
import numpy as np
from typing import Callable, Dict, List, Tuple, Any, Optional
from functools import wraps
import timeit


class Timer:
    
    def __init__(self, name : str = "Operation"):
        self.name = name
        self.start_time = None
        self.elapsed = None
    
    def __enter__(self):
        self.start_time = time.perf_counter()
        return self
    
    def __exit__(self, *args):
        self.elapsed = time.perf_counter() - self.start_time
        print(f"{self.name}: {self.elapsed:.6f} seconds")


def timeit_function(func : Callable, *args, n_runs : int = 10, **kwargs) -> Tuple[float, float, Any]:
    
    times = []
    result = None
    
    for _ in range(n_runs):
        start = time.perf_counter()
        result = func(*args, **kwargs)
        elapsed = time.perf_counter() - start
        times.append(elapsed)
    
    return np.mean(times), np.std(times), result


def benchmark_methods(methods : Dict[str, Callable],
                     problem_sizes : List[int],
                     setup_func : Callable,
                     n_runs : int = 5) -> Dict[str, List[float]]:
    
    results = {name: [] for name in methods}
    
    for size in problem_sizes:
        print(f"\nBenchmarking size {size}:")
        
        for method_name, method_func in methods.items():
            # setup problem
            problem = setup_func(size)
            
            # time method
            times = []
            for _ in range(n_runs):
                start = time.perf_counter()
                method_func(*problem)
                elapsed = time.perf_counter() - start
                times.append(elapsed)
            
            mean_time = np.mean(times)
            results[method_name].append(mean_time)
            print(f"  {method_name}: {mean_time:.6f} ± {np.std(times):.6f} s")
    
    return results


def profile_convergence(method : Callable,
                       problem : Tuple,
                       max_iterations : int = 100,
                       tol : float = 1e-6) -> Dict[str, Any]:
    
    errors = []
    times = []
    iteration_times = []
    
    start_total = time.perf_counter()
    
    for it, error, solution in method(*problem, max_iter=max_iterations, tol=tol):
        iter_start = time.perf_counter()
        
        errors.append(error)
        times.append(time.perf_counter() - start_total)
        
        if it > 0:
            iteration_times.append(time.perf_counter() - iter_start)
        
        if error < tol:
            break
    
    total_time = time.perf_counter() - start_total
    
    return {
        'iterations': len(errors),
        'errors': np.array(errors),
        'times': np.array(times),
        'iteration_times': np.array(iteration_times),
        'total_time': total_time,
        'converged': errors[-1] < tol,
        'final_error': errors[-1]
    }


def time_decorator(func: Callable) -> Callable:
    
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.perf_counter()
        result = func(*args, **kwargs)
        elapsed = time.perf_counter() - start
        print(f"{func.__name__} took {elapsed:.6f} seconds")
        return result
    return wrapper


def complexity_analysis(func : Callable,
                       sizes : List[int],
                       setup_func : Callable,
                       n_runs : int = 3) -> Tuple[np.ndarray, np.ndarray]:
    
    times = []
    
    for size in sizes:
        problem = setup_func(size)
        mean_time, _, _ = timeit_function(func, *problem, n_runs=n_runs)
        times.append(mean_time)
        print(f"Size {size}: {mean_time:.6f} s")
    
    return np.array(sizes), np.array(times)


def flops_counter(n : int, operation : str) -> int:
   
    operations = {
        'matvec': 2 * n**2,
        'matmul': 2 * n**3,
        'lu': (2/3) * n**3,
        'qr': 2 * n**3,
        'cholesky': (1/3) * n**3,
        'solve_triangular': n**2,
        'vector_norm': 2 * n,
        'dot_product': 2 * n,
    }
    
    if operation not in operations:
        raise ValueError(f"Unknown operation: {operation}. "
                        f"Available: {list(operations.keys())}")
    
    return int(operations[operation])


def efficiency_metric(time_seconds : float, flops : int) -> float:
    
    return flops / time_seconds


class PerformanceTracker:
    
    def __init__(self):
        self.measurements = []
    
    def record(self, name : str, time : float, size : int, 
              flops: Optional[int] = None):

        measurement = {
            'name': name,
            'time': time,
            'size': size,
            'flops': flops,
            'gflops': flops / time / 1e9 if flops else None
        }
        self.measurements.append(measurement)
    
    def summary(self) -> Dict[str, Dict[str, float]]:

        summary = {}
        
        for name in set(m['name'] for m in self.measurements):
            method_data = [m for m in self.measurements if m['name'] == name]
            times = [m['time'] for m in method_data]
            
            summary[name] = {
                'mean_time': np.mean(times),
                'std_time': np.std(times),
                'min_time': np.min(times),
                'max_time': np.max(times),
            }
            
            gflops = [m['gflops'] for m in method_data if m['gflops']]
            if gflops:
                summary[name]['mean_gflops'] = np.mean(gflops)
        
        return summary
    
    def print_summary(self):

        summary = self.summary()
        
        print("\n" + "="*60)
        print("PERFORMANCE SUMMARY")
        print("="*60)
        
        for name, stats in summary.items():
            print(f"\n{name}:")
            print(f"  Time: {stats['mean_time']:.6f} ± {stats['std_time']:.6f} s")
            if 'mean_gflops' in stats:
                print(f"  Performance: {stats['mean_gflops']:.2f} GFLOPS")