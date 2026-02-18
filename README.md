# RobustAsianPricer: High-Performance Monte Carlo Option Pricing : 
RobustAsianPricer is a high-performance C++20 engine designed to price Arithmetic Asian Options using Monte Carlo simulations.

This project serves a dual purpose:
- **High Performance Benchmark**: Demonstrates low-latency systems engineering, comparing multi-threaded CPU implementations against baseline sequential models.
- **Trustworthy AI Research**: Includes a "Robustness Quantification" module that injects adversarial market noise into simulation paths to stress-test the convergence and stability of Monte Carlo estimators.

## Core Engine (C++) : 

- **Arithmetic Asian Options**: Prices path-dependent options where the payoff depends on the average price over the option's life (unlike standard European options).

- **Mathematical Model**: Geometric Brownian Motion (GBM) utilizing Euler-Maruyama discretization.

- **High-Performance Computing**: 
    - Multi-threading: Scalable parallel execution using ```std::thread``` and ```std::future``` (Producer-Consumer pattern).
    - C++20: Utilizes Concepts, Coroutines (Generators), and Ranges for expressive, zero-overhead abstractions.
    - SIMD Optimization: Explicit vectorization (AVX2/AVX-512) for path generation (Planned).

- **Robustness & Analysis (Python Integration)**:
    - Adversarial Noise Injection: A configurable interface to perturb volatility surfaces and spot prices, simulating "black swan" micro-events to test model stability.
    - Data Pipeline: Fetches real-time market data (Spot, Risk-Free Rate, Implied Volatility) via ```yfinance```.
    - Visualization: Automated plotting of convergence rates and robustness metrics.

- **Tech Stack**:
    - Language: C++20
    - Build System: CMake (3.15+)
    - Testing: GoogleTest (Unit Tests), Google Benchmark (Latency/Throughput)
    - Scripting: Python 3 (Data fetching & plotting)
        - Dependencies: yfinance, numpy, matplotlib


## Build Instructions :

1. Fetch Market Data (Python)
```bash
python3 python/data_fetcher.py --ticker NVDA --expiry 2026-12-18
# Output: market_params.json
```

2. Run the Pricer (C++)
```bash
./build/RobustAsianPricer --config market_params.json --sims 1000000 --steps 252
```
3. Run Robustness Analysis
```bash
python3 python/robustness_analyzer.py --ticker NVDA --noise_level 0.05
```

## Performance Benchmarks
(Coming soon: Comparison charts of Single-threaded vs Multi-threaded vs GPU implementations)