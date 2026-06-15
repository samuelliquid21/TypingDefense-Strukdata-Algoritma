from PIL import Image
import os
import sys

frame_dir = os.path.join(os.path.dirname(__file__), '..', 'assets', 'video_frames')

# Convert all PNG frames to JPEG quality 88
total_before = 0
total_after = 0
count = 0

for fname in sorted(os.listdir(frame_dir)):
    if not fname.endswith('.png'):
        continue
    path = os.path.join(frame_dir, fname)
    total_before += os.path.getsize(path)

    img = Image.open(path).convert('RGB')
    jpg_name = fname.replace('.png', '.jpg')
    jpg_path = os.path.join(frame_dir, jpg_name)
    img.save(jpg_path, 'JPEG', quality=88, optimize=True)
    total_after += os.path.getsize(jpg_path)

    # Remove PNG
    os.remove(path)
    count += 1

print(f"Converted {count} frames")
print(f"Before (PNG): {total_before / 1024 / 1024:.2f} MB")
print(f"After  (JPEG q88): {total_after / 1024 / 1024:.2f} MB")
print(f"Saved: {(total_before - total_after) / 1024 / 1024:.2f} MB ({(1 - total_after/total_before)*100:.0f}%)")
