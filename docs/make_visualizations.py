"""Create self-contained SVG plots from verified C++ result CSV files.
No solver math is performed here; it only draws exported data for submission evidence.
"""
from pathlib import Path
import csv, math

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "docs" / "visualizations"
OUT.mkdir(exist_ok=True)

def svg(name, body, title, subtitle=""):
    text = f'''<svg xmlns="http://www.w3.org/2000/svg" width="1100" height="700" viewBox="0 0 1100 700">
<rect width="1100" height="700" fill="#f8fafc"/><style>text{{font-family:Arial,sans-serif;fill:#172554}} .t{{font-size:28px;font-weight:bold}} .s{{font-size:15px;fill:#475569}} .a{{font-size:14px;fill:#334155}} .grid{{stroke:#cbd5e1;stroke-width:1}} .axis{{stroke:#334155;stroke-width:2}}</style>
<text x="70" y="48" class="t">{title}</text><text x="70" y="74" class="s">{subtitle}</text>{body}</svg>'''
    (OUT / name).write_text(text, encoding="utf-8")

def axes(xlabel, ylabel):
    grid=''.join(f'<line x1="100" y1="{120+i*90}" x2="1020" y2="{120+i*90}" class="grid"/>' for i in range(6))
    return grid + '<line x1="100" y1="570" x2="1020" y2="570" class="axis"/><line x1="100" y1="120" x2="100" y2="570" class="axis"/>' + f'<text x="520" y="630" class="a">{xlabel}</text><text x="20" y="350" class="a" transform="rotate(-90 20,350)">{ylabel}</text>'

# Thermal curves.
curves=[]
for n,color in [(10,"#ef4444"),(20,"#f59e0b"),(40,"#10b981"),(80,"#2563eb")]:
    rows=list(csv.DictReader((ROOT/"results"/f"thermal_N{n}.csv").open()))
    points=' '.join(f'{100+920*float(r["x_m"]):.1f},{570-4.0*(float(r["temperature_FE_C"])-10):.1f}' for r in rows)
    curves.append(f'<polyline points="{points}" fill="none" stroke="{color}" stroke-width="2.5"/><text x="{760+(n//10)*35}" y="{120+(n//10)*25}" class="a" fill="{color}">FE N={n}</text>')
exact=[100-80*x+500*x*(1-x) for x in [i/100 for i in range(101)]]
exactpts=' '.join(f'{100+920*i/100:.1f},{570-4*(t-10):.1f}' for i,t in enumerate(exact))
labels=''.join(f'<text x="{95+230*i}" y="590" class="a">{i/4:.2f}</text>' for i in range(5)) + ''.join(f'<text x="65" y="{570-4*(v-10):.1f}" class="a">{v}</text>' for v in [20,40,60,80,100])
svg("thermal_temperature_curves.svg", axes("x (m)","Temperature (C)")+labels+f'<polyline points="{exactpts}" fill="none" stroke="#111827" stroke-width="3" stroke-dasharray="7 4"/><text x="770" y="105" class="a">dashed: analytical solution</text>'+''.join(curves), "Thermal FEA: FE temperature against analytical solution", "Uniform rod, L=1 m; k=10 W/(m K); q=10,000 W/m3; T(0)=100 C; T(L)=20 C")

# Convergence.
rows=list(csv.DictReader((ROOT/"results"/"thermal_convergence.csv").open()))
bars=[]
for i,r in enumerate(rows):
    err=float(r['max_midpoint_absolute_error_C']); h=420*(err/1.25); x=180+i*190
    bars.append(f'<rect x="{x}" y="{570-h:.1f}" width="100" height="{h:.1f}" rx="5" fill="#2563eb"/><text x="{x+30}" y="595" class="a">N={r["elements"]}</text><text x="{x+5}" y="{560-h:.1f}" class="a">{err:.5g} C</text>')
svg("thermal_mesh_convergence.svg", axes("Uniform element count","Maximum midpoint absolute error (C)")+''.join(bars)+"<text x='350' y='110' class='a'>Expected second-order midpoint convergence: error decreases approximately 4x per mesh doubling.</text>", "Thermal FEA mesh convergence", "Errors sampled at element midpoints using linear FE interpolation")

# Performance.
rows=list(csv.DictReader((ROOT/"results"/"performance.csv").open()))
values={p:[] for p in range(1,5)}
for r in rows: values[int(r['workers'])].append(float(r['compute_ms']))
med=[sorted(values[p])[len(values[p])//2] for p in range(1,5)]
bars=[]
for i,v in enumerate(med):
    h=420*v/max(med);x=180+i*190
    bars.append(f'<rect x="{x}" y="{570-h:.1f}" width="105" height="{h:.1f}" rx="5" fill="#7c3aed"/><text x="{x+26}" y="595" class="a">{i+1} worker</text><text x="{x+10}" y="{560-h:.1f}" class="a">{v:.3f} ms</text>')
svg("parallel_performance.svg",axes("CPU workers (five median repetitions)","Median compute time (ms)")+''.join(bars),"Parallel thermal batch benchmark","Seed 20261102; 20,000 cases; 80 elements; post-warm-up data only")

# Backprop loss.
rows=list(csv.DictReader((ROOT/"results"/"backprop_loss.csv").open()))
maxloss=float(rows[0]['loss']); pts=' '.join(f'{100+920*int(r["step"])/100:.1f},{570-420*float(r["loss"])/maxloss:.1f}' for r in rows)
svg("backprop_loss.svg",axes("Gradient descent step","Loss J = 0.5(y_hat - y)^2")+f'<polyline points="{pts}" fill="none" stroke="#dc2626" stroke-width="3"/><text x="680" y="130" class="a">tanh regression, simultaneous update, learning rate 0.1</text>',"Backpropagation training loss","Initial central finite-difference validation uses epsilon = 1e-6")

# Self-created STL geometry.
svg("geometry_ascii_stl.svg",'''<polygon points="260,520 820,510 540,180" fill="#93c5fd" stroke="#1d4ed8" stroke-width="5"/><circle cx="260" cy="520" r="7" fill="#1e3a8a"/><circle cx="820" cy="510" r="7" fill="#1e3a8a"/><circle cx="540" cy="180" r="7" fill="#1e3a8a"/><text x="200" y="550" class="a">vertex (0,0,0)</text><text x="770" y="540" class="a">vertex (1,0,0)</text><text x="510" y="160" class="a">vertex (0,1,0)</text><text x="430" y="370" class="a">one ASCII STL triangle</text>''',"Geometry view: validated self-created ASCII STL","Source units as recorded in example_ascii.stl; triangle count = 1")

# Temperature point cloud / legend.
nodes=list(csv.DictReader((ROOT/"data"/"synthetic_temperature_nodes.csv").open()))
colours=["#dc2626","#f59e0b","#2563eb"]
circles=[]
for r,c in zip(nodes,colours):
    x=180+700*float(r['x_m']);circles.append(f'<circle cx="{x}" cy="370" r="22" fill="{c}"/><text x="{x-35}" y="420" class="a">node {r["node_id"]}</text><text x="{x-35}" y="445" class="a">{r["temperature_C"]} C</text>')
legend=''.join(f'<rect x="{720+i*70}" y="150" width="70" height="25" fill="{c}"/>' for i,c in enumerate(colours))+'<text x="720" y="140" class="a">Temperature legend: hot to cold</text><text x="720" y="195" class="a">100 C       60 C        20 C</text>'
svg("temperature_point_cloud.svg",axes("x coordinate (m)","y / z coordinate (m)")+''.join(circles)+legend,"Temperature result view: synthetic nodal point cloud","CSV schema: node_id, x_m, y_m, z_m, temperature_C; units displayed in C")
print("Created", len(list(OUT.glob("*.svg"))), "SVG visualizations in", OUT)
