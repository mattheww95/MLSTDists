"""Create a distance matrix for benchmarking
"""
import random

cols = 100
rows = 1000

#columns = [int(random.random() * 1000) for _ in range(cols)]



print("name", *[i for i in range(cols)], sep="\t")
for i in range(rows):
    print(i, *[int(random.random() * 1000) for _ in range(cols)], sep="\t")
