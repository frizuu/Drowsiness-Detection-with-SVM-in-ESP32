import pandas as pd
import numpy as np

# UBAH: Langsung baca dataset mentah
df = pd.read_csv("dataset.csv")

WINDOW = 50
rows = []

for i in range(0, len(df)-WINDOW, WINDOW):
    chunk = df.iloc[i:i+WINDOW]
    row = {}
    
    row['mean_pitch'] = chunk['pitch'].mean()
    row['std_pitch'] = chunk['pitch'].std()
    row['max_pitch'] = chunk['pitch'].max()
    row['min_pitch'] = chunk['pitch'].min()
    row['mean_gx'] = chunk['gx'].mean()
    row['mean_gy'] = chunk['gy'].mean()
    row['mean_gz'] = chunk['gz'].mean()
    row['label'] = chunk['label'].mode()[0]
    
    rows.append(row)

feature_df = pd.DataFrame(rows)
feature_df.to_csv("feature_dataset.csv", index=False)
print("Ekstraksi fitur dari data mentah selesai!")