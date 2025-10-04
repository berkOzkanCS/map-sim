import numpy as np
import cv2
import pyvista as pv

def plot_map(png_file, txt_file):
    # --- Read metadata and threats from txt ---
    with open(txt_file) as f:
        bx, by, bz = map(float, f.readline().split())  # Map bounds
        sx_start, sy_start, sz_start = map(float, f.readline().split())  # Start position
        num_goals = int(f.readline())
        goals = [list(map(float, f.readline().split())) for _ in range(num_goals)]
        threats = [list(map(float, line.split())) for line in f if line.strip()]

    # --- Load heightmap from PNG ---
    heightmap = cv2.imread(png_file, cv2.IMREAD_GRAYSCALE)
    hx, hy = heightmap.shape

    # Scale heightmap to map bounds
    Z = heightmap.astype(float)
    # Z = np.flipud(heightmap.astype(float))
    x = np.linspace(0, bx, hy)
    y = np.linspace(0, by, hx)
    X, Y = np.meshgrid(x, y)

    # --- Create PyVista structured grid ---
    grid = pv.StructuredGrid(X, Y, Z)

    # --- Create plotter ---
    plotter = pv.Plotter()
    surf = plotter.add_mesh(grid, cmap="plasma", opacity=0.6, show_edges=False)

    # --- Add threats aligned with terrain ---
    for t in threats:
        if len(t) < 5:
            continue
        tx, ty, tz, R, H = t

        # print(t)

        # Align threat base with terrain
        # xi = int(round(tx / bx * (hy - 1)))
        # yi = int(round(ty / by * (hx - 1)))
        xi, yi = int(tx), int(ty)
        print(xi, yi)
        if 0 <= yi < hx and 0 <= xi < hy:
            terrain_z = Z[yi, xi]

        print(terrain_z, tz-H/2)
        cyl_center_z = tz # terrain_z + H / 2
        cyl = pv.Cylinder(center=(tx, ty, cyl_center_z), direction=(0,0,1), radius=R, height=H)
        plotter.add_mesh(cyl, color='b', opacity=0.7)

    # --- Add start and goals ---
    plotter.add_mesh(pv.Sphere(radius=bx*0.01, center=(sx_start, sy_start, sz_start)), color='g')
    for g in goals:
        plotter.add_mesh(pv.Sphere(radius=bx*0.01, center=g), color='y')

    # --- UI Controls ---
    plotter.add_axes()
    plotter.show_bounds(grid='back', location='outer')
    def update_opacity(value):
        surf.GetProperty().SetOpacity(value)
    plotter.add_slider_widget(update_opacity, [0.1, 1.0], value=0.6, title='Surface Opacity')

    # --- Show plot ---
    plotter.show()

# Usage
plot_map("heightmap.png", "map_data.txt")