from .norms import (
    vector_norm,
    matrix_norm,
    relative_error,
    relative_residual,
)

from .matrix_generators import (
    random_matrix,
    random_spd_matrix,
    hilbert_matrix,
    vandermonde_matrix,
    tridiagonal_matrix,
    poisson_matrix_1d,
    poisson_matrix_2d,
    diagonal_matrix,
    conditioned_matrix,
    random_sparse_matrix,
    linear_system,
    least_squares_problem,
)

from .plotting import (
    plot_convergence,
    plot_residual_history,
    plot_error_vs_stepsize,
    plot_spectrum,
    plot_singular_values,
    plot_matrix,
    plot_sparsity_pattern,
    plot_optimization_path,
    compare_methods,
    save_figure,
)

from .timing import (
    Timer,
    timeit_function,
    benchmark_methods,
    profile_convergence,
    time_decorator,
    complexity_analysis,
    flops_counter,
    efficiency_metric,
    PerformanceTracker,
)

from .finite_difference_checks import (
    finite_difference_gradient,
    check_gradient,
    finite_difference_hessian,
    check_hessian,
    directional_derivative_check,
    gradient_consistency_check,
)

__all__ = [
    
    'vector_norm',
    'matrix_norm',
    'relative_error',
    'relative_residual',

    'random_matrix',
    'random_spd_matrix',
    'hilbert_matrix',
    'vandermonde_matrix',
    'tridiagonal_matrix',
    'poisson_matrix_1d',
    'poisson_matrix_2d',
    'diagonal_matrix',
    'conditioned_matrix',
    'random_sparse_matrix',
    'linear_system',
    'least_squares_problem',

    'plot_convergence',
    'plot_residual_history',
    'plot_error_vs_stepsize',
    'plot_spectrum',
    'plot_singular_values',
    'plot_matrix',
    'plot_sparsity_pattern',
    'plot_optimization_path',
    'compare_methods',
    'save_figure',

    'Timer',
    'timeit_function',
    'benchmark_methods',
    'profile_convergence',
    'time_decorator',
    'complexity_analysis',
    'flops_counter',
    'efficiency_metric',
    'PerformanceTracker',

    'finite_difference_gradient',
    'check_gradient',
    'finite_difference_hessian',
    'check_hessian',
    'directional_derivative_check',
    'gradient_consistency_check',
]