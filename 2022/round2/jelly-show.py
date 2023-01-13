#!/usr/bin/env python
import sys
import plotly.graph_objects as go

def vlog(msg):
    sys.stderr.write("%s\n" % msg)

def line_to_xy(line: str) -> (int, int):
    ss = line.split()
    return (int(ss[0]), int(ss[1]))

def fget_n_xys(f, n: int) -> [(int, int)]:
    a = []
    while len(a) < n:
        line = f.readline()
        xy = line_to_xy(line)
        a.append(xy)
    return a

def figure_add_markers(fig, points, color, name):
    x = list(map(lambda xy: xy[0], points))
    y = list(map(lambda xy: xy[1], points))
    fig.add_scatter(x=x, y=y, 
                    mode="markers",
                    name=name,
                    marker=dict(size=16, color=color))
    

fn_in = sys.argv[1]
html_out = sys.argv[2]

fin = open(fn_in)
line_1 = fin.readline()
line_2 = fin.readline()
N = int(line_2.strip())
vlog(f"N={N}")
children = fget_n_xys(fin, N)
jelly = fget_n_xys(fin, 1)
sweets = fget_n_xys(fin, N)
fin.close()

fig = go.Figure()

figure_add_markers(fig, children, "blue", "Children")
figure_add_markers(fig, jelly, "red", "Jelly")
figure_add_markers(fig, sweets, "pink", "Sweets")

fig.write_html(html_out) 
sys.exit(0)
