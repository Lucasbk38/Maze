rm video.mp4
ffmpeg -r 20 -f image2 -i "img/img%d.bmp" -vcodec libx264 -crf 25 -pix_fmt yuv420p -vf scale=iw*4:4*ih:flags=neighbor -c:a libfaac video.mp4 -x264opts no-deblock -aq-mode 0
open video.mp4