rm -f output.mp4 out.nut 
ffmpeg  -i input.mp4 -c:v rawvideo -pix_fmt rgb24 -c:a pcm_u16le -s 10x10 -f nut  - > out.nut
# ffmpeg  -i input.mp4 -c:v rawvideo -c:a pcm_u16le -s 10x10 -f nut  - | ffmpeg -y -i - output.mp4
# ffmpeg  -s 10x10 -f rawvideo -i /dev/zero     -ar 44100 -ac 2 -f s16le -i /dev/zero     -c:v rawvideo -c:a pcm_u16le -f nut  - > out.nut 