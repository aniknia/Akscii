# generate a self-contained HTML slideshow
import glob, os

frames = sorted(glob.glob("img/output/*.txt"))
output = []
for frame in frames:
  with open(frame) as fp:
    output.append(fp.read())

font_size = 16
fps = 30

with open("img/output/video.html", "w") as viewer:
  viewer.write(f"""
    <!DOCTYPE html>
    <html>
      <body style="margin:10">
      <pre id="d" style="font-family: 'Noto Sans Mono', monospace; font-size: {font_size}px; line-height:1" />
        <script>
          const frames = {output};
          let i = 0;
          setInterval(() => {{ document.getElementById('d').textContent = frames[i++ % frames.length]; }}, {1000//fps});
        </script>
      </body>
    </html>
  """)