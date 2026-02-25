// FIX 1: specific headers instead of <bits/stdc++.h> (faster compile, standard practice)
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <algorithm> // for std::max
#include <chrono>

using namespace std;

struct PricingTask{
    int num_simulations; 
};

template <typename T>
class ThreadSafeQueue {
private:
    queue<T> q_internal; // Renamed from 'heap' (it's a FIFO queue, not a heap)
    mutable mutex mtx;
    condition_variable cv;
    bool stop_signal = false;

public:
    void push(T value) {
        {
            lock_guard<mutex> lock(mtx);
            q_internal.push(std::move(value));
        }
        cv.notify_one(); 
    }

    bool pop(T &value) {
        unique_lock<mutex> lock(mtx);
        
        // Wait until queue has items OR stop signal is received
        cv.wait(lock, [this]{
            return !q_internal.empty() || stop_signal;
        });

        // FIX 2: Double curly braces removed, logic kept same
        // If queue is empty AND we are told to stop, we are done.
        if(q_internal.empty() && stop_signal){
            return false; 
        }

        value = std::move(q_internal.front());
        q_internal.pop();
        return true;
    }
    
    // FIX 3: Renamed to match usage or update usage. Let's keep 'stop'
    void stop(){
        {
            lock_guard<mutex> lock(mtx);
            stop_signal = true;
        }
        cv.notify_all(); 
    }
};

struct OptionParams{
    double T;   // Changed 't' to 'T' to match finance convention
    double K;   
    double S0;  
    double sigma; 
    double r;   
};

// Global atomics
atomic<double> global_total_payoff{0.0}; 
atomic<long long> global_simulation_count{0}; 

void consumer_worker(ThreadSafeQueue<PricingTask> &task_queue, const OptionParams &params){
    // FIX 4: std::this_thread::get_id() requires 'this_thread' namespace
    hash<thread::id> hasher;
    auto seed = random_device{}() + hasher(this_thread::get_id());
    mt19937_64 rng(seed);
    normal_distribution<double> normal_dis(0.0, 1.0);

    // Pre-calculate outside the loop
    double drift = (params.r - 0.5 * params.sigma * params.sigma) * params.T; 
    double diffusion = params.sigma * sqrt(params.T);

    PricingTask task;
    while(task_queue.pop(task)){
        double local_batch_payoff = 0.0;

        for(int i=0; i<task.num_simulations; i++){
            double Z = normal_dis(rng); 
            double ST = params.S0 * exp(drift + diffusion * Z); 
            local_batch_payoff += max(ST - params.K, 0.0); 
        }

        // FIX 5: Missing closing parenthesis on the while loop
        double current = global_total_payoff.load();
        while(!global_total_payoff.compare_exchange_weak(current, current + local_batch_payoff));
        
        global_simulation_count += task.num_simulations; 
    }
}

void producer(ThreadSafeQueue<PricingTask> &queue, long long total_sims, int batch_size){
    long long sims_remaining = total_sims;

    while(sims_remaining > 0){
        // FIX 6: Cleaned up min() casting
        int current_batch = (int)std::min((long long)batch_size, sims_remaining);
        queue.push(PricingTask{current_batch});
        sims_remaining -= current_batch;
    }

    // FIX 7: The method in the class is called stop(), not stop_signal()
    queue.stop();  
}

int main() {
    const long long TOTAL_SIMS = 10000000;   
    const int BATCH_SIZE = 100000;           

    OptionParams params{
        .T = 1.0,       
        .K = 100.0,     
        .S0 = 100.0,    
        .sigma = 0.2,   
        .r = 0.05,      
    };

    ThreadSafeQueue<PricingTask> task_queue;

    cout << "Starting producer thread..." << endl;
    auto start_time = chrono::high_resolution_clock::now();

    unsigned int num_workers = thread::hardware_concurrency();
    
    // FIX 8: std::jthread requires C++20. Ensure you compile with -std=c++20
    vector<jthread> workers; 

    for(unsigned int i=0; i<num_workers; i++){
        // ref() and cref() are perfect here
        workers.emplace_back(consumer_worker, ref(task_queue), cref(params));
    }

    cout << "Launched " << num_workers << " consumer workers" << endl;

    // Run producer on main thread
    producer(task_queue, TOTAL_SIMS, BATCH_SIZE); 

    // FIX 9: Correct way to wait for jthreads
    // The previous empty loop did nothing. clearing the vector forces join.
    workers.clear(); 

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;

    // FIX 10: Atomic double cannot be divided directly, need .load()
    double final_total = global_total_payoff.load();
    double average_payoff = final_total / TOTAL_SIMS;
    double price = exp(-params.r * params.T) * average_payoff;

    cout << "-----------------------------\n";
    cout << "Estimated price: " << price << endl;
    cout << "Total simulations: " << global_simulation_count.load() << endl;
    cout << "Elapsed time: " << elapsed.count() << " seconds" << endl;
    
    return 0;
}