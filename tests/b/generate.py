import os

seed = os.sys.argv[1]
is_py = len(os.sys.argv) >= 3 and os.sys.argv[2] == '--py'

import string
import random

random.seed(seed)

vars = []
var_vals = []

for _ in range(10):
    print()

for ch in string.ascii_lowercase * 50:
    var = ch * 400
    var_val = random.choice([False, True])
    if is_py:
        var = var[0]
    vars.append(var)
    var_vals.append(var_vals)
    s = ' ' + var + ' = ' + str(var_val) + ' ; '.replace(' ' if not is_py else '?', ' ' * 150)
    if is_py:
        s = s.strip()
    print(s)
    for _ in range(10):
        print()

initial_q = ' not ( {} and {} or not ( {} xor ( {} ) ) ) '
q = initial_q
i = 0
for _ in range(25):
    q = q.format(vars[i], vars[i+1], vars[i+2], initial_q)
    i += 3

q = q.format(vars[i], vars[i+1], vars[i+2], 'not False')
q = q.replace(' ' if not is_py else '?', ' ' * 150)

if is_py:
    q = q.strip()
    q = q.replace('xor', '!=')
    q = 'print({})'.format(q)

print(q)

for _ in range(10):
    print()
