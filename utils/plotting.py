import numpy as np
import matplotlib.pyplot as plt
from typing import List, Optional, Tuple, Dict
from matplotlib.figure import Figure
from matplotlib.axes import Axes


def plot_convergence(errors : np.ndarray, 
                    iterations : Optional[np.ndarray] = None,
                    title : str = "Convergence",
                    xlabel : str = "Iteration",
                    ylabel : str = "Error",
                    yscale : str = "log",
                    reference_rates : Optional[Dict[str, float]] = None,
                    ax : Optional[Axes] = None) -> Axes:
    
    if ax is None:
        fig, ax = plt.subplots(figsize=(8, 6))
    
    if iterations is None:
        iterations = np.arange(len(errors))
    
    ax.plot(iterations, errors, 'o-', linewidth=2, markersize=6, label='Actual')
    
    # add reference convergence rates
    if reference_rates:
        for label, rate in reference_rates.items():
            ref_line = errors[0] * (rate ** iterations)
            ax.plot(iterations, ref_line, '--', alpha=0.6, label=f'{label} (rate={rate})')
    
    ax.set_xlabel(xlabel, fontsize=12)
    ax.set_ylabel(ylabel, fontsize=12)
    ax.set_title(title, fontsize=14)
    ax.set_yscale(yscale)
    ax.grid(True, alpha=0.3)
    ax.legend()
    
    return ax


def plot_residual_history(residuals : List[float],
                         title : str = "Residual History",
                         ax : Optional[Axes] = None) -> Axes:

    return plot_convergence(
        np.array(residuals),
        title=title,
        ylabel="Residual Norm ||r||",
        ax=ax
    )


def plot_error_vs_stepsize(stepsizes : np.ndarray,
                          errors : np.ndarray,
                          expected_order : Optional[int] = None,
                          title : str = "Error vs Step Size",
                          ax : Optional[Axes] = None) -> Axes:

    if ax is None:
        fig, ax = plt.subplots(figsize=(8, 6))
    
    ax.loglog(stepsizes, errors, 'o-', linewidth=2, markersize=6, label='Actual')
    
    if expected_order is not None:
        # reference line: error ~ h^p
        ref_errors = errors[0] * (stepsizes / stepsizes[0]) ** expected_order
        ax.loglog(stepsizes, ref_errors, '--', alpha=0.6, 
                 label=f'$O(h^{expected_order})$')
    
    ax.set_xlabel('Step Size $h$', fontsize=12)
    ax.set_ylabel('Error', fontsize=12)
    ax.set_title(title, fontsize=14)
    ax.grid(True, alpha=0.3)
    ax.legend()
    
    return ax


def plot_spectrum(eigenvalues : np.ndarray,
                 title : str = "Eigenvalue Spectrum",
                 ax : Optional[Axes] = None) -> Axes:

    if ax is None:
        fig, ax = plt.subplots(figsize=(8, 8))
    
    real = np.real(eigenvalues)
    imag = np.imag(eigenvalues)
    
    ax.scatter(real, imag, alpha=0.6, s=50)
    ax.axhline(y=0, color='k', linewidth=0.5, alpha=0.3)
    ax.axvline(x=0, color='k', linewidth=0.5, alpha=0.3)
    ax.set_xlabel('Real Part', fontsize=12)
    ax.set_ylabel('Imaginary Part', fontsize=12)
    ax.set_title(title, fontsize=14)
    ax.grid(True, alpha=0.3)
    ax.set_aspect('equal')
    
    return ax


def plot_singular_values(singular_values : np.ndarray,
                        title : str = "Singular Value Spectrum",
                        ax: Optional[Axes] = None) -> Axes:

    if ax is None:
        fig, ax = plt.subplots(figsize=(8, 6))
    
    ax.semilogy(range(1, len(singular_values) + 1), singular_values, 'o-', 
                linewidth=2, markersize=6)
    ax.set_xlabel('Index', fontsize=12)
    ax.set_ylabel('Singular Value', fontsize=12)
    ax.set_title(title, fontsize=14)
    ax.grid(True, alpha=0.3)
    
    return ax


def plot_matrix(A : np.ndarray,
               title : str = "Matrix Structure",
               cmap : str = "RdBu_r",
               ax : Optional[Axes] = None) -> Axes:

    if ax is None:
        fig, ax = plt.subplots(figsize=(8, 8))
    
    im = ax.imshow(A, cmap=cmap, aspect='auto', interpolation='nearest')
    ax.set_title(title, fontsize=14)
    ax.set_xlabel('Column Index', fontsize=12)
    ax.set_ylabel('Row Index', fontsize=12)
    plt.colorbar(im, ax=ax)
    
    return ax


def plot_sparsity_pattern(A : np.ndarray,
                         title : str = "Sparsity Pattern",
                         ax : Optional[Axes] = None) -> Axes:
 
    if ax is None:
        fig, ax = plt.subplots(figsize=(8, 8))
    
    # convert to dense if sparse
    if hasattr(A, 'toarray'):
        A = A.toarray()
    
    ax.spy(A, markersize=1)
    ax.set_title(title, fontsize=14)
    ax.set_xlabel('Column Index', fontsize=12)
    ax.set_ylabel('Row Index', fontsize=12)
    
    return ax


def plot_optimization_path(x_history : np.ndarray,
                          contour_func: Optional[callable] = None,
                          xlim : Tuple[float, float] = (-5, 5),
                          ylim : Tuple[float, float] = (-5, 5),
                          title : str = "Optimization Path",
                          ax : Optional[Axes] = None) -> Axes:

    if ax is None:
        fig, ax = plt.subplots(figsize=(10, 8))
    
    # draw contours if function provided
    if contour_func is not None:
        x = np.linspace(xlim[0], xlim[1], 200)
        y = np.linspace(ylim[0], ylim[1], 200)
        X, Y = np.meshgrid(x, y)
        Z = contour_func(X, Y)
        ax.contour(X, Y, Z, levels=20, alpha=0.4, cmap='viridis')
    
    # plot trajectory
    ax.plot(x_history[:, 0], x_history[:, 1], 'o-', 
           linewidth=2, markersize=6, color='red', alpha=0.7)
    ax.plot(x_history[0, 0], x_history[0, 1], 'go', 
           markersize=10, label='Start')
    ax.plot(x_history[-1, 0], x_history[-1, 1], 'r*', 
           markersize=15, label='End')
    
    ax.set_xlim(xlim)
    ax.set_ylim(ylim)
    ax.set_xlabel('$x_1$', fontsize=12)
    ax.set_ylabel('$x_2$', fontsize=12)
    ax.set_title(title, fontsize=14)
    ax.legend()
    ax.grid(True, alpha=0.3)
    
    return ax


def compare_methods(results : Dict[str, np.ndarray],
                   metric : str = "Error",
                   title : str = "Method Comparison",
                   yscale : str = "log",
                   ax : Optional[Axes] = None) -> Axes:
   
    if ax is None:
        fig, ax = plt.subplots(figsize=(10, 6))
    
    for method_name, values in results.items():
        iterations = np.arange(len(values))
        ax.plot(iterations, values, 'o-', linewidth=2, 
               markersize=5, label=method_name, alpha=0.8)
    
    ax.set_xlabel('Iteration', fontsize=12)
    ax.set_ylabel(metric, fontsize=12)
    ax.set_title(title, fontsize=14)
    ax.set_yscale(yscale)
    ax.grid(True, alpha=0.3)
    ax.legend()
    
    return ax


def save_figure(fig : Figure, filename : str, dpi : int = 300, bbox_inches : str = 'tight'):
   
    fig.savefig(filename, dpi=dpi, bbox_inches=bbox_inches)
    print(f"Figure saved to {filename}")
