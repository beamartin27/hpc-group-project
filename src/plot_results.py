import pandas as pd
import matplotlib.pyplot as plt
import glob
import os

def load_data(pattern):
    files = glob.glob(pattern)
    dfs = []
    for f in files:
        try:
            df = pd.read_csv(f)
            dfs.append(df)
        except Exception as e:
            print(f"Error reading {f}: {e}")
    if not dfs:
        return pd.DataFrame()
    return pd.concat(dfs, ignore_index=True)

def plot_strong_scaling(df):
    if df.empty:
        print("No data for strong scaling.")
        return
    
    # Filter or sort
    df = df.sort_values(by='ranks')
    
    # Calculate speedup
    # Assuming the smallest rank count is the baseline
    baseline = df.iloc[0]
    t0 = baseline['elapsed_sec']
    df['speedup'] = t0 / df['elapsed_sec']
    df['ideal_speedup'] = df['ranks'] / baseline['ranks']
    
    plt.figure(figsize=(10, 6))
    plt.plot(df['ranks'], df['speedup'], 'o-', label='Measured Speedup')
    plt.plot(df['ranks'], df['ideal_speedup'], '--', label='Ideal Speedup')
    plt.xlabel('Number of MPI Ranks')
    plt.ylabel('Speedup')
    plt.title('Strong Scaling (Fixed Total Problem Size)')
    plt.legend()
    plt.grid(True)
    plt.savefig('results/plots/strong_scaling.png')
    print("Saved results/plots/strong_scaling.png")

    # Efficiency
    df['efficiency'] = df['speedup'] / (df['ranks'] / baseline['ranks'])
    plt.figure(figsize=(10, 6))
    plt.plot(df['ranks'], df['efficiency'], 'o-', color='orange')
    plt.xlabel('Number of MPI Ranks')
    plt.ylabel('Parallel Efficiency (0-1)')
    plt.title('Strong Scaling Efficiency')
    plt.ylim(0, 1.1)
    plt.grid(True)
    plt.savefig('results/plots/strong_scaling_efficiency.png')
    print("Saved results/plots/strong_scaling_efficiency.png")

def plot_weak_scaling(df):
    if df.empty:
        print("No data for weak scaling.")
        return
    
    df = df.sort_values(by='ranks')
    
    # For weak scaling, ideal time is constant
    plt.figure(figsize=(10, 6))
    plt.plot(df['ranks'], df['elapsed_sec'], 'o-', label='Measured Time')
    plt.axhline(y=df['elapsed_sec'].iloc[0], color='r', linestyle='--', label='Ideal Time')
    plt.xlabel('Number of MPI Ranks')
    plt.ylabel('Time (seconds)')
    plt.title('Weak Scaling (Fixed Work per Rank)')
    plt.legend()
    plt.grid(True)
    plt.savefig('results/plots/weak_scaling.png')
    print("Saved results/plots/weak_scaling.png")

    # Efficiency: T1 / TN
    t0 = df['elapsed_sec'].iloc[0]
    df['efficiency'] = t0 / df['elapsed_sec']
    
    plt.figure(figsize=(10, 6))
    plt.plot(df['ranks'], df['efficiency'], 'o-', color='green')
    plt.xlabel('Number of MPI Ranks')
    plt.ylabel('Weak Scaling Efficiency')
    plt.ylim(0, 1.1)
    plt.grid(True)
    plt.savefig('results/plots/weak_scaling_efficiency.png')
    print("Saved results/plots/weak_scaling_efficiency.png")

def main():
    os.makedirs('results/plots', exist_ok=True)
    
    # Load Strong Scaling Data
    # Assuming file naming convention: mc_strong_N*.csv or similar
    # We might need to adjust this based on actual output filenames
    strong_df = load_data('results/mc_strong_*.csv')
    # Also include baseline if it's part of strong scaling (1 node)
    baseline_df = load_data('results/mc_baseline_*.csv')
    
    if not strong_df.empty and not baseline_df.empty:
        strong_df = pd.concat([baseline_df, strong_df], ignore_index=True)
    elif strong_df.empty and not baseline_df.empty:
        strong_df = baseline_df
        
    # Deduplicate if multiple runs exist, take mean or latest?
    # For now, let's just group by ranks and take mean
    if not strong_df.empty:
        strong_agg = strong_df.groupby('ranks', as_index=False).mean(numeric_only=True)
        plot_strong_scaling(strong_agg)
    
    # Load Weak Scaling Data
    weak_df = load_data('results/mc_weak_*.csv')
    # Baseline is usually also the start of weak scaling
    if not weak_df.empty and not baseline_df.empty:
        weak_df = pd.concat([baseline_df, weak_df], ignore_index=True)
    elif weak_df.empty and not baseline_df.empty:
        weak_df = baseline_df
        
    if not weak_df.empty:
        weak_agg = weak_df.groupby('ranks', as_index=False).mean(numeric_only=True)
        plot_weak_scaling(weak_agg)

if __name__ == "__main__":
    main()
