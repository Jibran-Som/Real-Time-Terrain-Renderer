from PIL import Image
import os

def generate_cubemap(source_image_path, output_dir="cubemap_output", face_size=1024):
    """
    Generate a cube map from a source image.
    
    Args:
        source_image_path: Path to the source image
        output_dir: Directory to save cube map faces
        face_size: Size of each cube face (square)
    """
    
    # Load the source image
    img = Image.open(source_image_path)
    img_width, img_height = img.size
    
    # Create output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)
    
    # Calculate positions for cube faces from the source image
    # This assumes a standard cube map layout (cross format)
    # Adjust these coordinates based on your source image layout
    
    # Assuming source image is in a 4x3 grid (common for cube maps)
    # +-----+-----+-----+-----+
    # |     |  Y+ |     |     |
    # |  X- |  Z+ |  X+ |  Z- |
    # |     |  Y- |     |     |
    # +-----+-----+-----+-----+
    
    # Face coordinates (left, top, right, bottom)
    # You'll need to adjust these based on your specific source image layout
    
    # For a 4x3 grid (12 images in total, with cube faces)
    if img_width >= face_size * 4 and img_height >= face_size * 3:
        # Assuming standard 4x3 cube map layout
        faces = {
            'right': (face_size * 2, face_size, face_size * 3, face_size * 2),     # X+
            'left': (0, face_size, face_size, face_size * 2),                      # X-
            'top': (face_size, 0, face_size * 2, face_size),                       # Y+
            'bottom': (face_size, face_size * 2, face_size * 2, face_size * 3),    # Y-
            'front': (face_size, face_size, face_size * 2, face_size * 2),         # Z+
            'back': (face_size * 3, face_size, face_size * 4, face_size * 2)       # Z-
        }
    else:
        # If image doesn't match expected layout, use percentage-based cropping
        # This is useful if you have a single image that needs to be divided
        print(f"Image size: {img_width}x{img_height}")
        print("Assuming image contains cube map faces in custom layout")
        
        # Example: Divide image into 6 equal parts horizontally
        # You'll need to customize this based on your source image
        face_width = img_width // 6
        faces = {
            'right': (face_width * 0, 0, face_width * 1, face_width),
            'left': (face_width * 1, 0, face_width * 2, face_width),
            'top': (face_width * 2, 0, face_width * 3, face_width),
            'bottom': (face_width * 3, 0, face_width * 4, face_width),
            'front': (face_width * 4, 0, face_width * 5, face_width),
            'back': (face_width * 5, 0, face_width * 6, face_width)
        }
    
    # Generate each face
    for face_name, box in faces.items():
        left, top, right, bottom = box
        
        # Clamp to image bounds
        left = max(0, left)
        top = max(0, top)
        right = min(img_width, right)
        bottom = min(img_height, bottom)
        
        # Crop and resize to square if needed
        cropped = img.crop((left, top, right, bottom))
        
        # Ensure face is square
        if cropped.width != cropped.height:
            # Resize to square while maintaining aspect ratio
            size = min(cropped.width, cropped.height)
            cropped = cropped.resize((face_size, face_size), Image.Resampling.LANCZOS)
        elif cropped.width != face_size:
            cropped = cropped.resize((face_size, face_size), Image.Resampling.LANCZOS)
        
        # Save face
        output_path = os.path.join(output_dir, f"{face_name}.jpg")
        cropped.save(output_path)
        print(f"Saved {face_name} face as {output_path}")
    
    # Create a combined image for preview
    create_combined_preview(output_dir, face_size)
    
    print(f"\nCube map generation complete! Files saved in '{output_dir}' directory")

def create_combined_preview(output_dir, face_size):
    """Create a combined preview image of all cube faces."""
    
    # Create a new image with all faces arranged
    preview = Image.new('RGB', (face_size * 4, face_size * 3))
    
    face_positions = {
        'right': (face_size * 2, face_size),
        'left': (0, face_size),
        'top': (face_size, 0),
        'bottom': (face_size, face_size * 2),
        'front': (face_size, face_size),
        'back': (face_size * 3, face_size)
    }
    
    for face_name, position in face_positions.items():
        face_path = os.path.join(output_dir, f"{face_name}.jpg")
        if os.path.exists(face_path):
            face_img = Image.open(face_path)
            preview.paste(face_img, position)
    
    preview_path = os.path.join(output_dir, "cubemap_preview.jpg")
    preview.save(preview_path)
    print(f"Created combined preview: {preview_path}")

def generate_cubemap_from_single_region(source_image_path, output_dir="cubemap_single", region_box=None):
    """
    Generate a cube map from a single rectangular region of an image.
    This divides the region into 6 equal square parts.
    
    Args:
        source_image_path: Path to source image
        output_dir: Output directory
        region_box: Region to crop from source (left, top, right, bottom)
    """
    
    img = Image.open(source_image_path)
    img_width, img_height = img.size
    
    # If no region specified, use the whole image
    if region_box is None:
        region_box = (0, 0, img_width, img_height)
    
    left, top, right, bottom = region_box
    
    # Clamp to image bounds
    left = max(0, left)
    top = max(0, top)
    right = min(img_width, right)
    bottom = min(img_height, bottom)
    
    # Crop the region
    region = img.crop((left, top, right, bottom))
    region_width, region_height = region.size
    
    # Divide region into 6 horizontal strips
    os.makedirs(output_dir, exist_ok=True)
    
    face_width = region_width // 6
    face_size = min(face_width, region_height)
    
    face_names = ['right', 'left', 'top', 'bottom', 'front', 'back']
    
    for i, face_name in enumerate(face_names):
        face_left = i * face_width
        face_right = (i + 1) * face_width
        face_top = 0
        face_bottom = region_height
        
        # Crop face
        face_box = (face_left, face_top, face_right, face_bottom)
        face = region.crop(face_box)
        
        # Resize to square
        face = face.resize((face_size, face_size), Image.Resampling.LANCZOS)
        
        # Save face
        output_path = os.path.join(output_dir, f"{face_name}.jpg")
        face.save(output_path)
        print(f"Saved {face_name} face as {output_path}")

# Usage examples:

generate_cubemap("1232063.jpg", "cubemap_output", face_size=1024)

