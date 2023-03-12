from pysmt.shortcuts import Symbol, Or, And, Not, GE, GT, get_model, Equals, TRUE, Int, is_sat
from pysmt.typing import BOOL,INT

from functools import reduce
from itertools import product

from util import Permuter

#flag = hxp{BFGSQ0C0DUV0EJM1G0I0Z1K0LST1N0P0Z0QR0YZ4WX3}
inp = "BFGQS0C0DUV0EJM1G0I0Z1K0LST1N0P0Z0QR0YZ4WX3"

def get_graph(inp):
    graph = []
    cur = []
    while inp:
        skp = ord(inp[0]) - 0x30
        if skp < 10:
            graph.append(cur)
            for i in range(0, skp):
                graph.append([])
            cur = []
        else:
            cur.append(ord(inp[0]) - 0x41)
        inp = inp[1:]
    graph.append([])
    adj_mat = [[False for _ in range(len(graph))] for _ in range(len(graph))]
    for node, edges in enumerate(graph):
        for edge in edges:
            adj_mat[node][edge] = True
            adj_mat[edge][node] = True
    return adj_mat

def toDot(graph):
    alp = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    print("graph {")
    for node, edges in enumerate(graph):
        for edge, _ in filter(lambda x: x[1] and x[0] > node, enumerate(edges)):
            print(f"\t{alp[node]} -- {alp[edge]};")
    print("}")

def sbfs(graph, s):
    cur = {s}
    visited = set()
    res = []
    while cur:
        nxt = set()
        for e in sorted(list(cur)):
            res.append(e)
            nxt.update({t for t, c in enumerate(graph[e]) if c})
        visited.update(cur)
        cur = nxt - visited
    return res

def flag_builder(acc, e):
    ret, skip = acc
    if e:
        return (ret + str(skip) + e, 0)
    else:
        return (ret, skip + 1)

def flag_from_graph(g):
    short = ["".join(chr(j+0x41) for j, c in enumerate(r) if j > i and c) for i, r in enumerate(g)]
    #print(short)
    txtgraph = reduce(flag_builder, short[1:], (short[0], 0))
    #print(txtgraph)
    return f"hxp{{{txtgraph[0]}{txtgraph[1] - 1}}}"


graph = [[Symbol(f"edge_{chr(0x41 + i)}{chr(0x41 + j)}", BOOL) for j in range(26)] for i in range(26)]
dists = [[Symbol(f"dist_{chr(0x41 + i)}{chr(0x41 + j)}", INT) for j in range(26)] for i in range(26)]

dim = len(graph)
assert dim == len(dists)
for row in graph:
    assert dim == len(row)
for row in dists:
    assert dim == len(row)

# A -> B   A - B - E
# A -> C   |   |
# B -> D   C   D

bfs0 = [0,1,2,3,4]
bfs1 = [1,0,3,4,2]
bfs2 = [2,0,1,3,4]
bfs3 = [3,1,0,4,2]
bfs4 = [4,1,0,3,2]

# A - B - E
# |   | / 
# C   D

#sbfs =  [ [0,1,2,3,4]
#        , [1,0,3,4,2]
#        , [2,0,1,3,4]
#        , [3,1,4,0,2]
#        , [4,1,3,0,2]]

f = TRUE()
for i in range(dim):
    for j in range(dim):
        if i != j:
            f = (f & graph[i][j].Iff(Equals(dists[i][j], Int(1))) &
                graph[i][j].Iff(graph[j][i]) &
                dists[i][j].Equals(dists[j][i])
            )
            
            #triangle_inequality = And(GE(dists[i][j] + dists[j][k], dists[i][k]) for k in range(dim) if i != k)
            #f = f & triangle_inequality
            neib_adj_clause = And(Equals(dists[i][k], dists[j][k] + Int(1)) | Equals(dists[i][k], dists[j][k]) | Equals(dists[i][k] + Int(1), dists[j][k]) for k in range(dim) if i != k)
            f = (f & Equals(dists[i][j], Int(1)).Implies(neib_adj_clause))
        else:
            f = f & Not(graph[i][j]) & Equals(dists[i][j], Int(0))

def getbfs(bfs):
    start = bfs[0]
    fst = bfs[1]
    f = Equals(dists[start][fst], Int(1)) & (graph[start][fst])
    for a,b in zip(bfs[1:], bfs[2:]):
        f = f & GE(dists[start][b], dists[start][a])
        if b < fst:
            f = f & GT(dists[start][b], dists[start][fst])
    return f

def reduce_help(tmp, b):
    xs, last, acc = tmp
    if b < last:
        return (xs + [acc], b, [b])
    else:
        return (xs, b, acc + [b])

def getsrtbfs(bfs):
    start = bfs[0]
    fst = bfs[1]
    f = Equals(dists[start][fst], Int(1)) & (graph[start][fst])

    #print(bfs[1:])
    xs, _, acc = reduce(reduce_help, bfs[2:], ([], bfs[1], [bfs[1]]))
    xs.append(acc)
    #print(xs)
    for l0, l1 in zip(xs, xs[1:]):
        for a in l1:
            f = f & Or(graph[a][b] for b in l0 + l1 if a != b)
    for l0, l1 in zip(xs, xs[2:]):
        for a, b in product(l0, l1):
            f = f & Not(graph[a][b])

    #for l0, l1 in zip(xs, xs[1:]):
    #    for a, b in product(l0, l1):
    #        f = f & GT(dists[start][a], dists[start][b])
    for a,b in zip(bfs[1:], bfs[2:]):
        if b < a:
            f = f & GT(dists[start][b], dists[start][a])
        else:
            f = f & GE(dists[start][b], dists[start][a])
    return f

def get_solution(f):
    m = get_model(f)
    if m is None:
        return None, None
    dsol = Or(Or(Not(graph[i][j]) if m.get_py_value(graph[i][j]) else graph[i][j] for j in range(len(graph))) for i in range(len(graph)))
    return (m, f & dsol)

if __name__ == "__main__":
    g = get_graph(inp)
    print(f"[+] read graph")
    sps = "HXPREVFASTNOW"
    bfss = [sbfs(g, ord(l) - 0x41) for l in sps]
    print(f"[+] create bfss for starting points: \"{sps}\"")
    #for bfs in bfss:
    #    print(f"\t, {{{','.join(map(lambda x: str(x ^ 42),bfs))}}}")
    m, nf = get_solution(f & And(getsrtbfs(b) for b in bfss))
    assert m is not None and not is_sat(nf)
    print(f"[+] found unique solution")
    gt = [[m.get_py_value(e) for e in n] for n in graph]
    print(f"[+] flag: {flag_from_graph(gt)}")
    assert g == gt
    print(f"[+] solution matches input")
    pm = Permuter()
    pm.pa = [25 - i for i in range(25)]
    pm2 = Permuter()
    pm2.pa = [i + 1 for i in range(25)]
    wbfss = []
    ibfss = []
    for bfs in bfss:
        ibfs = pm.iperm(bfs)
        ibfss.append(ibfs[:])
        pm.perm(ibfs)
        assert ibfs == bfs
        tmp = ibfs[:]
        pm2.perm(tmp)
        wbfss.append(tmp)
    print("[+] create permuted array:")
    for ibfs in ibfss:
        print(f"\t, {{{','.join(map(lambda x: str(x ^ 42),ibfs))}}}")
    assert(not is_sat(f & And(getsrtbfs(pb) for pb in ibfss)))
    assert(not is_sat(f & And(getsrtbfs(pb) for pb in wbfss)))
    print("[+] chall for permuted arrays is unsat")

# kirschju timer: 3:25
