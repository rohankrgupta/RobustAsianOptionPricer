#include <bits/stdc++.h>
#include <random>
using namespace std;

#define fast_io             ios_base::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
#define F                   first
#define S                   second
#define MP                  make_pair
#define all(o)              (o).begin(), (o).end()
#define mset(m,v)           memset(m,v,sizeof(m))
#define fr(i,n)             for(lli i=0;i<(n);++i)
#define rep(i,a,b)          for(lli i=a;i<=b;++i)
#define endl                '\n'
#define int                 long long
#define sz(x)               (lli)(x).size()
#define per(i,a,b)          for(lli i=a;i>=b;i--)
#define MOD                 1000000007
#define pb                  push_back
#define pii                 pair<int,int>

typedef long long lli;        typedef long double ld;
typedef pair<lli,lli> ii;     typedef vector<lli> vi;
typedef vector<ii> vii;       typedef vector<vi> graph;
long double EPS=1e-9;

lli binpow(lli b,lli p,lli mod){lli ans=1;b%=mod;for(;p;p>>=1){if(p&1)ans=ans*b%mod;b=b*b%mod;}return ans;}

#ifndef ONLINE_JUDGE
#define debarr(a,n)cerr<<#a<<":";for(int i=0;i<n;i++)cerr<<a[i]<<" ";cerr<<endl;
#define debmat(mat,row,col)cerr<<#mat<<":\n";for(int i=0;i<row;i++){for(int j=0;j<col;j++)cerr<<mat[i][j]<<" ";cerr<<endl;}
#define pr(...)dbs(#__VA_ARGS__,__VA_ARGS__)
template<class S,class T>ostream &operator<<(ostream &os,const pair<S,T> &p){return os<<"("<<p.first<<","<<p.second<<")";}
template<class T>ostream &operator<<(ostream &os,const vector<T> &p){os<<"[";for(auto&it:p)os<<it<<" ";return os<<"]";}
template<class T>ostream &operator<<(ostream &os,const set<T>&p){os<<"[";for(auto&it:p)os<<it<<" ";return os<<"]";}
template<class T>ostream &operator<<(ostream &os,const multiset<T>&p){os<<"[";for(auto&it:p)os<<it<<" ";return os<<"]";}
template<class S,class T>ostream &operator<<(ostream &os,const map<S,T>&p){os<<"[";for(auto&it:p)os<<it<<" ";return os<<"]";}
template<class T>void dbs(string str,T t){cerr<<str<<":"<<t<<"\n";}
template<class T,class...S>void dbs(string str,T t,S... s){int idx=str.find(',');cerr<<str.substr(0,idx)<<":"<<t<<",";dbs(str.substr(idx+1),s...);}
#else
#define pr(...){}
#define debarr(a,n){}
#define debmat(mat,row,col){}
#endif

//--------------------------------------------------------------------------------------------------------------------------------------------------------------//
const bool tests=0;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------//

struct OptionParams{
  double T;   // time to maturity
  double K;   // strike price
  double S0;  // initial stock price
  double sigma ; // volatility
  double r;   // risk-free interest rate
  int N;   // number of steps in the monte carlo simulation
};

// pricing a european call option using monte carlo simulation
double price_euro_call(const OptionParams& params){
    // random no generation 
    mt19937_64 rng(random_device{}());
    normal_distribution<double> normal_dis(0.0,1.0);

    // precalc drift to avoid inside loop
    double drift = (params.r - 0.5 * params.sigma * params.sigma) * params.T;
    double diffusion = params.sigma * sqrt(params.T);
    double payoff = 0.0;

    for(int i=0; i<params.N; i++){
        // generate a standard normal random variable
        double Z = normal_dis(rng); // standard normal random variable
        
        // simulate the stock price at maturity
        double ST = params.S0 * exp(drift + diffusion * Z);

        // calculate the payoff of the call option
        payoff += max(ST - params.K, 0.0);

    }

    // average payoff and discount back to present value
    double average_payoff = payoff / params.N;
    double discounted_price = average_payoff * exp(-params.r * params.T);

    return discounted_price;
}

signed main()
{
    OptionParams params{
        .T = 1.0,       // 1 year to maturity
        .K = 100.0,     // strike price
        .S0 = 100.0,    // initial stock price
        .sigma = 0.2,   // volatility
        .r = 0.05,      // risk-free interest rate
        .N = 100000     // number of simulations  
    };

    cout << "Running monte carlo simulation to price a European call option..." << endl;
    double option_price = price_euro_call(params);
    cout << "Estimated price of the European call option: " << option_price << endl;

    std::cout << "Parameters:\n";
    std::cout << "  Spot: " << params.S0 << ", Strike: " << params.K << "\n";
    std::cout << "  Vol:  " << params.sigma << ", Rate:   " << params.r << "\n";
    std::cout << "---------------------------\n";
    std::cout << "Estimated Price: " << price << "\n";
    return 0;
}

















































