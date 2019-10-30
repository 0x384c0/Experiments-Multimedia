all:
	$(MAKE) test

setup_mac_os:
	# xcode-select --install
	brew install gst-plugins-good gst-plugins-bad
	

uninstall_mac_os:
	# brew tap ggpeti/rmrec
	brew rmrec gst-plugins-bad
	brew rmrec gst-plugins-good


test:
	echo "Audio Output aac mpegts"
	gst-launch-1.0 audiotestsrc wave=sine num-buffers=100 ! audioconvert ! faac ! mpegtsmux ! fdsink | ffplay -autoexit -i -
	echo "Video Input h264 mpegts"
	ffmpeg -i ~/Movies/test.mp4 -t 00:05 -f mpegts - | gst-launch-1.0 -v fdsrc ! fdsink | ffplay -autoexit -i -
	echo "Video Output h264 mpegts"
	gst-launch-1.0 -q videotestsrc pattern=ball num-buffers=100 !  vtenc_h264 ! video/x-h264,width=320,height=240,framerate=30/1 ! h264parse ! mpegtsmux ! fdsink | ffplay -autoexit -i -
	echo "Video Output raw video"
	gst-launch-1.0 -q videotestsrc pattern=ball num-buffers=100 ! video/x-raw,height=320,width=240,framerate=30/1,format=RGB ! fdsink | ffplay -autoexit -f rawvideo -pixel_format rgb24 -video_size 240x320 -i -

	# echo "Video Audio Output Mux mpegts"
	# gst-launch-1.0 \
	# 	audiotestsrc wave=sine num-buffers=100 ! audioconvert ! faac ! mux. !\
	# 	videotestsrc pattern=ball num-buffers=100 !  vtenc_h264 ! video/x-h264,width=320,height=240,framerate=30/1 ! h264parse ! mux. ! \
	# 	mpegtsmux name=mux ! fdsink # | ffplay -autoexit -i -