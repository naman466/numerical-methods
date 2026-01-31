import numpy as np
from typing import Callable, Tuple, Optional


def finite_difference_gradient(f : Callable, x: np.ndarray, 
                               epsilon : float = 1e-5,
                               method : str = 'central') -> np.ndarray:

    x = np.asarray(x, dtype=float)
    n = len(x)
    grad = np.zeros(n)
    
    if method == 'forward':
        f0 = f(x)
        for i in range(n):
            x_plus = x.copy()
            x_plus[i] += epsilon
            grad[i] = (f(x_plus) - f0) / epsilon
            
    elif method == 'central':
        for i in range(n):
            x_plus = x.copy()
            x_minus = x.copy()
            x_plus[i] += epsilon
            x_minus[i] -= epsilon
            grad[i] = (f(x_plus) - f(x_minus)) / (2 * epsilon)
            
    elif method == 'complex':
        # complex step derivative: no subtractive cancellation
        for i in range(n):
            x_complex = x.astype(complex)
            x_complex[i] += 1j * epsilon
            grad[i] = np.imag(f(x_complex)) / epsilon
    else:
        raise ValueError(f"Unknown method: {method}")
    
    return grad


def check_gradient(f : Callable, grad_f : Callable, x : np.ndarray,
                  epsilon : float = 1e-5,
                  tol : float = 1e-4,
                  method : str = 'central',
                  verbose : bool = True) -> Tuple[bool, float]:
   
    grad_analytical = grad_f(x)
    grad_numerical = finite_difference_gradient(f, x, epsilon, method)
    
    diff = grad_analytical - grad_numerical
    norm_diff = np.linalg.norm(diff)
    norm_analytical = np.linalg.norm(grad_analytical)
    norm_numerical = np.linalg.norm(grad_numerical)
    
    # compute relative error
    if norm_analytical > 0:
        relative_error = norm_diff / norm_analytical
    else:
        relative_error = norm_diff
    
    passed = relative_error < tol
    
    if verbose:
        print("="*60)
        print("GRADIENT CHECK")
        print("="*60)
        print(f"Method: {method}")
        print(f"Epsilon: {epsilon}")
        print(f"||grad_analytical||: {norm_analytical:.6e}")
        print(f"||grad_numerical||:  {norm_numerical:.6e}")
        print(f"||difference||:      {norm_diff:.6e}")
        print(f"Relative error:      {relative_error:.6e}")
        print(f"Tolerance:           {tol:.6e}")
        print(f"Status: {'✓ PASSED' if passed else '✗ FAILED'}")
        
        if not passed and norm_diff > 0:
            # show worst components
            abs_diff = np.abs(diff)
            worst_idx = np.argmax(abs_diff)
            print(f"\nWorst component: index {worst_idx}")
            print(f"  Analytical: {grad_analytical[worst_idx]:.6e}")
            print(f"  Numerical:  {grad_numerical[worst_idx]:.6e}")
            print(f"  Difference: {diff[worst_idx]:.6e}")
        
        print("="*60)
    
    return passed, relative_error


def finite_difference_hessian(f : Callable, x: np.ndarray,
                              epsilon : float = 1e-5) -> np.ndarray:
   
    x = np.asarray(x, dtype=float)
    n = len(x)
    H = np.zeros((n, n))
    
    f0 = f(x)
    
    # Diagonal elements : ∂²f/∂xᵢ²
    for i in range(n):
        x_plus = x.copy()
        x_minus = x.copy()
        x_plus[i] += epsilon
        x_minus[i] -= epsilon
        
        H[i, i] = (f(x_plus) - 2*f0 + f(x_minus)) / (epsilon**2)
    
    # off diagonal elements : ∂²f/∂xᵢ∂xⱼ
    for i in range(n):
        for j in range(i+1, n):
            x_pp = x.copy()
            x_pm = x.copy()
            x_mp = x.copy()
            x_mm = x.copy()
            
            x_pp[i] += epsilon
            x_pp[j] += epsilon
            
            x_pm[i] += epsilon
            x_pm[j] -= epsilon
            
            x_mp[i] -= epsilon
            x_mp[j] += epsilon
            
            x_mm[i] -= epsilon
            x_mm[j] -= epsilon
            
            H[i, j] = (f(x_pp) - f(x_pm) - f(x_mp) + f(x_mm)) / (4 * epsilon**2)
            H[j, i] = H[i, j]  # symmetry
    
    return H


def check_hessian(f : Callable, hess_f : Callable, x : np.ndarray,
                 epsilon : float = 1e-4,
                 tol : float = 1e-3,
                 verbose : bool = True) -> Tuple[bool, float]:
    
    hess_analytical = hess_f(x)
    hess_numerical = finite_difference_hessian(f, x, epsilon)
    
    diff = hess_analytical - hess_numerical
    norm_diff = np.linalg.norm(diff, 'fro')
    norm_analytical = np.linalg.norm(hess_analytical, 'fro')
    
    if norm_analytical > 0:
        relative_error = norm_diff / norm_analytical
    else:
        relative_error = norm_diff
    
    passed = relative_error < tol
    
    if verbose:
        print("="*60)
        print("HESSIAN CHECK")
        print("="*60)
        print(f"Epsilon: {epsilon}")
        print(f"||Hess_analytical||_F: {norm_analytical:.6e}")
        print(f"||Hess_numerical||_F:  {np.linalg.norm(hess_numerical, 'fro'):.6e}")
        print(f"||difference||_F:      {norm_diff:.6e}")
        print(f"Relative error:        {relative_error:.6e}")
        print(f"Tolerance:             {tol:.6e}")
        print(f"Status: {'✓ PASSED' if passed else '✗ FAILED'}")
        print("="*60)
    
    return passed, relative_error


def directional_derivative_check(f : Callable, grad_f : Callable,
                                 x : np.ndarray, d : np.ndarray,
                                 alphas : Optional[np.ndarray] = None,
                                 verbose : bool = True) -> np.ndarray:
    
    if alphas is None:
        alphas = np.logspace(-1, -8, 15)
    
    f0 = f(x)
    grad_x = grad_f(x)
    directional_deriv = np.dot(grad_x, d)
    
    errors = []
    
    for alpha in alphas:
        f_alpha = f(x + alpha * d)
        predicted = f0 + alpha * directional_deriv
        error = abs(f_alpha - predicted)
        
        # normalize by |f(x + αd) - f(x)|
        diff = abs(f_alpha - f0)
        relative_error = error / diff if diff > 0 else error
        
        errors.append(relative_error)
    
    if verbose:
        print("="*60)
        print("DIRECTIONAL DERIVATIVE CHECK")
        print("="*60)
        print(f"{'Alpha':>12} {'Relative Error':>15} {'Status':>10}")
        print("-"*60)
        
        for alpha, error in zip(alphas, errors):
            status = '✓' if error < 0.01 else '✗'
            print(f"{alpha:>12.6e} {error:>15.6e} {status:>10}")
        
        print("="*60)
        print("Note: Error should decrease linearly with alpha")
        print("="*60)
    
    return np.array(errors)


def gradient_consistency_check(grad_f : Callable, x : np.ndarray,
                               n_samples : int = 10,
                               epsilon : float = 1e-7,
                               verbose: bool = True) -> bool:
    
    grad_base = grad_f(x)
    
    variations = []
    
    for _ in range(n_samples):
        perturbation = epsilon * np.random.randn(len(x))
        x_perturbed = x + perturbation
        grad_perturbed = grad_f(x_perturbed)
        
        diff = np.linalg.norm(grad_perturbed - grad_base)
        variations.append(diff)
    
    mean_variation = np.mean(variations)
    max_variation = np.max(variations)
    
    # heuristic: variation should be O(epsilon)
    expected_variation = epsilon * np.linalg.norm(grad_base)
    passed = max_variation < 10 * expected_variation
    
    if verbose:
        print("="*60)
        print("GRADIENT CONSISTENCY CHECK")
        print("="*60)
        print(f"Samples:           {n_samples}")
        print(f"Perturbation:      {epsilon:.6e}")
        print(f"||grad(x)||:       {np.linalg.norm(grad_base):.6e}")
        print(f"Mean variation:    {mean_variation:.6e}")
        print(f"Max variation:     {max_variation:.6e}")
        print(f"Expected:          {expected_variation:.6e}")
        print(f"Status: {'✓ PASSED' if passed else '⚠ WARNING'}")
        print("="*60)
    
    return passed