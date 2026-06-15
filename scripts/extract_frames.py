"""
Extract GIF frames as PNG files with optional frame skip.
Frame skip = 1 keeps all frames, 2 keeps every other frame, etc.
"""
import os
from PIL import Image

gif_path = os.path.join(os.path.dirname(__file__), '..', 'assets', 'sound', 'ASCII_VIDEO.gif')
out_dir = os.path.join(os.path.dirname(__file__), '..', 'assets', 'video_frames')

frame_skip = int(os.environ.get('FRAME_SKIP', '1'))

os.makedirs(out_dir, exist_ok=True)

img = Image.open(gif_path)
w, h = img.size
print(f"GIF: {w}x{h}, mode={img.mode}, frame_skip={frame_skip}")

frame = 0
out_idx = 0
while True:
    try:
        img.seek(frame)
    except EOFError:
        break
    if frame % frame_skip == 0:
        fname = os.path.join(out_dir, f"frame_{out_idx:04d}.png")
        img.save(fname, optimize=True)
        out_idx += 1
    frame += 1

total_frames = out_idx
original_count = frame
print(f"Original frames: {original_count}, extracted: {total_frames}")

total_size = sum(os.path.getsize(os.path.join(out_dir, f)) for f in os.listdir(out_dir))
print(f"Total size: {total_size / 1024 / 1024:.2f} MB")
print(f"Avg per frame: {total_size / total_frames / 1024:.1f} KB")
