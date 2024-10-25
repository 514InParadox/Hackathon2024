import atexit
import json
import os
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
os.remove(f'{dataset}.gz')

start = time.process_time_ns()
os.system(f'./{comp} {dataset} out/')
end = time.process_time_ns()
t1 = end - start

with open(f'{dataset}', 'r', encoding='utf-8') as f:
    j0 = [json.loads(line) for line in f]
for i, f0 in enumerate(sorted(os.listdir('out'))):
    os.system(f'./{decomp} {out}/{f0} {out}/{i}.txt')
    j1 = []
    i = 0
    while True:
        try:
            with open(f'{dataset}_{i}', 'r', encoding='utf-8') as f:
                for line in f:
                    j1.append(json.loads(line))
        except:
            break
        i += 1
atexit.register(clean)
if j0 != j1:
    print('Wrong output')
    sys.exit(1)
if t0 < 2 * t1:
    print('Time limit exceeded')
    sys.exit(1)
print('Pass')
