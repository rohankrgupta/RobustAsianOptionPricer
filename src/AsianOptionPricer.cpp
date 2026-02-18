#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <numeric>   // for partial_sum
#include <algorithm> // for transform
#include <iomanip>

// C++20 Constants
#include <numbers>

using namespace std;

// Utility for creating a linspace (similar to R's seq or plot axes)
vector<double> linspace(double start, double end, int num) {
    vector<double> linest(num);
    if (num == 0) return linest;
    if (num == 1) { linest[0] = start; return linest; }
    double delta = (end - start) / (num - 1);
    for(int i = 0; i < num; ++i) {
        linest[i] = start + delta * i;
    }
    return linest;
}

class MonteCarloSimulation {
private:
    mt19937_64 rng; // 64-bit Mersenne Twister

public:
    MonteCarloSimulation() {
        random_device rd;
        rng.seed(rd());
    }

    // ---------------------------------------------------------
    // 1. Simple Random Walk (Corresponds to Slide 26)
    // ---------------------------------------------------------
    // R Logic: x <- sign(p - z); S[t+1] <- S[t] + x[t]
    vector<double> simple_random_walk(int Nt, double p = 0.5) {
        uniform_real_distribution<double> dist(0.0, 1.0);
        vector<double> path(Nt + 1, 0.0); // Start at 0

        for (int t = 0; t < Nt; ++t) {
            double z = dist(rng);
            // Matches R: sign(p - z). If z < p, result is +1, else -1.
            double step = (z < p) ? 1.0 : -1.0; 
            path[t + 1] = path[t] + step;
        }
        return path;
    }

    // ---------------------------------------------------------
    // 2. Lognormal Price Process (Corresponds to Slide 30)
    // ---------------------------------------------------------
    // R Logic: r <- rnorm(mean=mu*dt, sd=sigma*sqrt(dt)); S <- cumsum(r); P <- exp(S)
    vector<double> lognormal_process(int Nt, double mu, double sigma, double dt, double P0 = 1.0) {
        // Prepare distribution N(mu*dt, sigma*sqrt(dt))
        double drift = mu * dt;
        double diffusion = sigma * sqrt(dt);
        normal_distribution<double> dist(drift, diffusion);

        vector<double> log_returns(Nt);
        vector<double> prices(Nt + 1);
        
        prices[0] = P0;
        double current_log_price = log(P0);

        // Simulate
        for (int t = 0; t < Nt; ++t) {
            double r_t = dist(rng); // Generate random log return
            current_log_price += r_t;
            prices[t + 1] = exp(current_log_price);
        }
        return prices;
    }

    // ---------------------------------------------------------
    // 3. AR(1) Process (Corresponds to Slide 36)
    // ---------------------------------------------------------
    // R Logic: R[t] <- (1+lambda)*(mu*dt) - lambda*R[t-1] + epsilon[t]
    // Note: Slide 36 uses 'R' for the process variable and 'r' for log(1+R).
    vector<double> ar1_process(int Nt, double lambda, double mu, double sigma, double dt, double R0 = 0.0) {
        // Noise term epsilon ~ N(0, sigma*sqrt(dt))
        normal_distribution<double> dist(0.0, sigma * sqrt(dt));
        
        vector<double> R(Nt + 1);
        R[0] = R0;

        // Precompute constant drift term from slide formula
        // Formula: (1 + lambda) * (mu * dt)
        double drift_const = (1.0 + lambda) * (mu * dt);

        for (int t = 1; t <= Nt; ++t) {
            double epsilon = dist(rng);
            // Recursive AR(1) definition
            R[t] = drift_const - (lambda * R[t - 1]) + epsilon;
        }
        return R;
    }
};

int main() {
    MonteCarloSimulation mc;
    
    // 

    // --- Simulation 1: Simple Random Walk ---
    cout << "--- 1. Simple Random Walk (Slide 26) ---\n";
    int steps_rw = 20;
    auto rw_path = mc.simple_random_walk(steps_rw, 0.5);
    
    cout << "Time: ";
    for (int i=0; i<=steps_rw; ++i) cout << setw(3) << i << " ";
    cout << "\nPath: ";
    for (double val : rw_path) cout << setw(3) << val << " ";
    cout << "\n\n";

    // --- Simulation 2: Lognormal Price Process ---
    cout << "--- 2. Lognormal Price Process (Slide 30) ---\n";
    // Parameters from Slide 30
    double sigma = 0.3;
    double mu = 0.1;
    double dt = 1.0 / 252.0; 
    int steps_ln = 10; // Reduced for printing, typically 252
    
    auto ln_path = mc.lognormal_process(steps_ln, mu, sigma, dt, 1.0);
    
    cout << fixed << setprecision(4);
    cout << "First 10 steps of GBM Price Path:\n";
    for (size_t i = 0; i < ln_path.size(); ++i) {
        cout << "t=" << i << ": " << ln_path[i] << "\n";
    }
    cout << "\n";

    // --- Simulation 3: AR(1) Process ---
    cout << "--- 3. AR(1) Process (Slide 36) ---\n";
    // Parameters from Slide 36
    double lambda_ar = 0.4;
    double mu_ar = 0.1;
    // Note: Slide 36 uses sigma * sqrt(dt) for noise. 
    // Assuming the same sigma/dt conventions as previous slides.
    
    auto ar_path = mc.ar1_process(steps_ln, lambda_ar, mu_ar, sigma, dt);

    cout << "First 10 steps of AR(1) Path:\n";
    for (size_t i = 0; i < ar_path.size(); ++i) {
        cout << "t=" << i << ": " << ar_path[i] << "\n";
    }

    return 0;
}