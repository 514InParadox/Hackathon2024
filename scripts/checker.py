# Only use in the same directory of the dataset

import atexit
import json
import os
import shutil
import sys
import time

def clean():
    shutil.rmtree('out')

_, comp, decomp, dataset = sys.argv
path = os.path.dirname(__file__)

start = time.process_time_ns()
os.system(f'gzip -k {dataset}')
end = time.process_time_ns()
t0 = end - start
try:
    os.remove(f'{dataset}.gz')
except:
    pass

start = time.process_time_ns()
os.system(f'{comp} {dataset} out')
end = time.process_time_ns()
t1 = end - start

with open(f'{dataset}', 'r', encoding='utf-8') as f:
    j0 = [json.loads(line) for line in f]
j1 = []
i = 0
while os.path.exists(f'out/{dataset}_{i}'):
    os.system(f'{decomp} out/{dataset}_{i} out/{i}.txt')
    with open(f'out/{i}.txt', 'r', encoding='utf-8') as f:
        for line in f:
            j1.append(json.loads(line))
    i += 1
atexit.register(clean)
if j0 != j1:
    with open('ans.txt', 'w', encoding='utf-8') as f:
        json.dump(j0, f)
    with open('my.txt', 'w', encoding='utf-8') as f:
        json.dump(j1, f)
    print('Wrong output')
    sys.exit(1)
if t0 < 2 * t1:
    print('Time limit exceeded')
    sys.exit(1)
print('Pass')
