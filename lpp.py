import gurobipy as gp
from gurobipy import GRB
from collections import defaultdict
import sys

# Check if the correct number of arguments are provided
if len(sys.argv) != 2:
    print("Usage: python lpp.py <filename>")
    sys.exit(1)

# Extract the filename from command-line arguments
filename = sys.argv[1]

# Number of time slots needed for each chunk before a given deadline {chunk_id:{d1:T1, d2:T2,..}}
chunk_deadlines = defaultdict(dict)
chunk_list = set()
deadline_list = set()
F = {}

# Overestimating total number of machines
N = 0
B = 0
S = 0

try:
    # Open the file in read mode
    with open(filename, 'r') as file:
        # Read B, S, and K from file
        B = int(file.readline().strip())
        S = int(file.readline().strip())
        tot_jobs = int(file.readline().strip())
        # Read job details
        for _ in range(tot_jobs):
            job_id, deadline, chunks_accessed = map(int, file.readline().split())
            deadline_list.add(deadline)
            N += chunks_accessed
            chunk_ids = list(map(int, file.readline().split()))
            for chunk in chunk_ids:
                chunk_list.add(chunk)
                chunk_deadlines[chunk][deadline] = chunk_deadlines[chunk].get(deadline, 0) + 1

except FileNotFoundError:
    print(f"Error: File '{filename}' not found.")
    sys.exit(1)

model = gp.Model()
model.params.LogToConsole = 0
objective = gp.LinExpr(0)
variable_cnt = 0
time_reqd = defaultdict(int)
active_node = {}
placement_vars = {}
M = 1e6 # Large constant

# Active node
for machines in range(1,N+1):
    active_node[machines] = model.addVar(vtype=GRB.BINARY, name=f'U_{machines}')
    variable_cnt += 1


for machines in range(1,N+1):
    expr = 0
    for chunk in chunk_list:
        placement_vars[(chunk,machines)] = model.addVar(vtype=GRB.BINARY, name=f'P_{chunk}_{machines}')
        expr += placement_vars[(chunk,machines)]
        variable_cnt += 1
    # Add placement constraint
    model.addConstr(expr <= B*active_node[machines])

for chunk in chunk_list:
    for deadline in sorted(chunk_deadlines[chunk].keys()):
        time_reqd[chunk] += chunk_deadlines[chunk].get(deadline, 0)
        expr = 0
        for machines in range(1,N+1):
            F[(chunk,machines,deadline)] = model.addVar(vtype=GRB.INTEGER, lb=0, name=f'F_{chunk}_{machines}_{deadline}')
            expr += F[(chunk,machines,deadline)]
            variable_cnt += 1
            # Activity constraint
            model.addConstr(F[(chunk,machines,deadline)] <= deadline * (placement_vars[(chunk,machines)]))
            model.addConstr(F[(chunk,machines,deadline)] <= deadline * (active_node[machines]))
        # Add deadline constraint
        model.addConstr(expr >= time_reqd[chunk])

for machines in range(1,N+1):
    for deadline in deadline_list:
        expr = 0
        for chunk in chunk_list:
            # Computation constraint
            expr += F.get((chunk,machines,deadline),0)
        model.addConstr(expr <= deadline*active_node[machines]*S)

for machines in range(1,N+1):
    objective+= active_node[machines] 

model.setObjective(objective,GRB.MINIMIZE)
model.optimize()
#model.write("h.lp")

# Post Processing
active_machines = [] # For getting list of active machines
machine_to_chunks = defaultdict(list)

if model.status == GRB.OPTIMAL:
    # Print the objective value
    print('Number of active Nodes:', model.objVal)

    # Get the active nodes
    for machines in range(1, N + 1):
        if active_node[machines].x == 1:
            active_machines.append(machines)

    # Get the machines assigned in each chunks
    for chunk, machines in placement_vars:
        if placement_vars[(chunk, machines)].x == 1:
            #print(f'Chunk {chunk} assigned to machine {machines}')
            machine_to_chunks[machines].append(chunk)

    # Print the chunks in each machines
    for machines in active_machines:
        print(f"Chunks mapped to Machine {machines}:",machine_to_chunks[machines])

    # Get the number of assigned slots
    for chunk, machines, deadline in F:
        if F[(chunk, machines, deadline)].x > 0:
            print(f'Timeslots for chunk {chunk} in machine {machines} before deadline {deadline}: {F[(chunk, machines, deadline)].x}')
else:
    print('Cant find a solution to this problem')