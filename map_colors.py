# map_colors.py
from PIL import Image
import sys

def map_colors(particles_file, image_file, output_file):
    # Read particle positions
    particles = []
    with open(particles_file, 'r') as f:
        count = int(f.readline().strip())
        for line in f:
            parts = line.strip().split()
            if len(parts) >= 6:
                particles.append({
                    'id': int(parts[0]),
                    'x': float(parts[1]),
                    'y': float(parts[2]),
                    # Original colors (we'll replace these)
                    'r': int(parts[3]),
                    'g': int(parts[4]),
                    'b': int(parts[5])
                })
    
    print(f"Loaded {len(particles)} particles")
    
    # Load image
    img = Image.open(image_file).convert('RGB')
    img_width, img_height = img.size
    print(f"Image size: {img_width}x{img_height}")
    
    # Get simulation bounds from particle positions
    min_x = min(p['x'] for p in particles)
    max_x = max(p['x'] for p in particles)
    min_y = min(p['y'] for p in particles)
    max_y = max(p['y'] for p in particles)
    
    print(f"Particle bounds: X[{min_x:.1f}, {max_x:.1f}] Y[{min_y:.1f}, {max_y:.1f}]")
    
    # Map each particle position to image pixel
    with open(output_file, 'w') as f:
        for p in particles:
            # Normalize position to [0, 1]
            norm_x = (p['x'] - min_x) / (max_x - min_x) if max_x > min_x else 0.5
            norm_y = (p['y'] - min_y) / (max_y - min_y) if max_y > min_y else 0.5
            
            # Map to image coordinates
            img_x = int(norm_x * (img_width - 1))
            img_y = int(norm_y * (img_height - 1))
            
            # Clamp to valid range
            img_x = max(0, min(img_width - 1, img_x))
            img_y = max(0, min(img_height - 1, img_y))
            
            # Get pixel color
            r, g, b = img.getpixel((img_x, img_y))
            
            # Write: id r g b
            f.write(f"{p['id']} {r} {g} {b}\n")
    
    print(f"Saved colors to {output_file}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python map_colors.py <particles.txt> <image.png> [output.txt]")
        print("Example: python map_colors.py particles.txt myimage.png colors.txt")
        sys.exit(1)
    
    particles_file = sys.argv[1]
    image_file = sys.argv[2]
    output_file = sys.argv[3] if len(sys.argv) > 3 else "colors.txt"
    
    map_colors(particles_file, image_file, output_file)