import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import norm
from scipy.optimize import curve_fit

def gaussian(x, mean, sigma, amplitude):
    """Define a Gaussian function for curve fitting."""
    return amplitude * np.exp(-0.5 * ((x - mean) / sigma) ** 2)

def fit1(entries=1000, save=False):
    # Generate random samples from a Gaussian distribution
    mean, sigma = 50, 10
    data = np.random.normal(mean, sigma, entries)
    
    # Create a histogram of the data
    bins = 100
    bin_edges = np.linspace(0, 100, bins + 1)
    hist, edges = np.histogram(data, bins=bin_edges)
    
    # Calculate bin centers and errors
    bin_centers = 0.5 * (edges[:-1] + edges[1:])
    bin_errors = np.sqrt(hist)  # Poisson errors assuming count data

    # protect curvefit against bins with simga=0
    mask = bin_errors > 0
    hist = hist[mask]
    bin_centers = bin_centers[mask]
    bin_errors =  bin_errors[mask]
    
    # Plot the histogram
    plt.errorbar(bin_centers, hist, yerr=bin_errors, fmt='o', label='Data (with errors)', ecolor='black', capsize=2)

    # Perform Gaussian fit
    initial_guess = [mean, sigma, max(hist)]  # Initial guess for curve_fit
    popt, pcov = curve_fit(gaussian, bin_centers, hist, p0=initial_guess, sigma=bin_errors, absolute_sigma=True)
    fit_mean, fit_sigma, fit_amplitude = popt
    fit_errors = np.sqrt(np.diag(pcov))  # Extract parameter uncertainties

    # Plot the fitted Gaussian
    x_fit = np.linspace(0, 100, 1000)
    y_fit = gaussian(x_fit, *popt)
    plt.plot(x_fit, y_fit, label='Gaussian Fit', color='red')

    # Display fit results
    print("\nFit Params and errors")
    print(f'Amplitude: {fit_amplitude:.2f} ± {fit_errors[2]:.2f}')
    print(f'Mean: {fit_mean:.2f} ± {fit_errors[0]:.2f}')
    print(f'Sigma: {fit_sigma:.2f} ± {fit_errors[1]:.2f}')

    # Calculate chi-squared and p-value
    residuals = hist - gaussian(bin_centers, *popt)
    chi2 = np.sum((residuals / bin_errors) ** 2)
    dof = len(hist) - len(popt)  # Degrees of freedom
    p_value = 1 - norm.cdf(chi2, dof)
    print(f'Chi-Squared: {chi2:.2f}, Degrees of Freedom: {dof}, P-Value: {p_value:.2f}')

    # Finalize the plot
    plt.title('Gaussian Fit to Random Data')
    plt.xlabel('x')
    plt.ylabel('Frequency')
    plt.legend()
    plt.grid(True)

    # Save the plot if requested
    if save:
        plt.savefig('fit_results.png')
        print("Plot saved as 'fit_results.png'")

    print("Close the plot to exit")
    plt.show()
   

if __name__ == "__main__":
    fit1()

