import bpy
import os
import math



OUTPUT_DIR = bpy.path.abspath("//cubemap_8192x4096_V2")  # Output folder
RESOLUTION = 4096  # Each face will be square (so final cubemap 8192x4096 in cross layout)
SAMPLES = 64        # Cycles samples (adjust for quality vs speed)

# -----------------------------
# SETUP SCENE
# -----------------------------
scene = bpy.context.scene
scene.render.engine = 'CYCLES'
scene.cycles.device = 'CPU'  # CPU rendering (Use GPU if GPU is better)
scene.cycles.samples = SAMPLES

scene.render.resolution_x = RESOLUTION
scene.render.resolution_y = RESOLUTION
scene.render.image_settings.file_format = 'OPEN_EXR'
scene.render.image_settings.color_depth = '32'

# Create output folder if needed
os.makedirs(OUTPUT_DIR, exist_ok=True)

# Ensure we have a camera at the center
if not scene.camera:
    bpy.ops.object.camera_add(location=(0, 0, 0))
    cam = bpy.context.active_object
    scene.camera = cam
else:
    cam = scene.camera
    cam.location = (0, 0, 0)

cam.data.type = 'PERSP'
cam.data.lens = 18  # 90° FOV

# -----------------------------
# CUBEMAP FACE ROTATIONS
# -----------------------------
faces = [
     ("front",   (0, -90, 0)),  # +X
     ("back",    (0, 90, 0)),   # -X
     ("right",     (-90, 0, 0)),  # +Y
     ("left",  (90, 0, 0)),   # -Y
     ("bottom",   (0, 0, 0)),    # +Z
     ("top",    (0, 180, 0)),  # -Z
]

# -----------------------------
# RENDER EACH FACE
# -----------------------------
for name, rotation in faces:
    cam.rotation_euler = (
        math.radians(rotation[0]),
        math.radians(rotation[1]),
        math.radians(rotation[2])
    )
    
    bpy.context.view_layer.update()
    
    output_path = os.path.join(OUTPUT_DIR, f"{name}.exr")
    scene.render.filepath = output_path
    
    print(f"Rendering {name}...")
    bpy.ops.render.render(write_still=True)
    print(f"Saved {name} → {output_path}")

print("\nAll cubemap faces rendered successfully!")
print(f"Output directory: {OUTPUT_DIR}")
