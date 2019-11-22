all:
	$(MAKE) test

setup_mac_os:
	# xcode-select --install
	brew install gst-plugins-good gst-plugins-bad
	gst-inspect-1.0

	cd gst-webkit &&\
	./autogen.sh &&\
	brew install webkitgtk &&\
	./configure
	

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

	# TODO: findout, why didnt work, if audiotestsrc comes first
	echo "Video Audio Output Mux mpegts"
	gst-launch-1.0 \
		videotestsrc pattern=ball num-buffers=100							! queue ! videoconvert ! vtenc_h264 ! video/x-h264,width=320,height=240,framerate=30/1 ! h264parse ! mux. \
		audiotestsrc wave=ticks sine-periods-per-tick=100 num-buffers=100	! queue ! audioconvert ! faac ! mux. \
		mpegtsmux name=mux ! fdsink | ffplay -autoexit -i -

	echo "Video chroma keying"
	gst-launch-1.0 videotestsrc pattern=smpte75 num-buffers=100 ! mixer.sink_0 \
		videotestsrc pattern=ball foreground-color=0xFFFFFF num-buffers=100 ! \
		alpha method=custom target-r=255 target-g=255 target-b=255 ! mixer.sink_1 \
		videomixer name=mixer sink_0::zorder=0 sink_1::zorder=1 ! \
		videoconvert ! vtenc_h264 ! video/x-h264,width=320,height=240,framerate=30/1 ! h264parse ! mpegtsmux ! fdsink | ffplay -autoexit -i -

FILTER_NAME="CustomFilter"
SOURCE_NAME="gstcustomfilter"
setup_filter:
	mkdir $(FILTER_NAME) &&\
	git clone https://gitlab.freedesktop.org/gstreamer/gst-template.git --depth 1 || true &&\
	cd gst-template/gst-plugin/src &&\
	../tools/make_element $(FILTER_NAME) &&\
	mv $(SOURCE_NAME)".c" ../../../$(FILTER_NAME)/ &&\
	mv $(SOURCE_NAME)".h" ../../../$(FILTER_NAME)/
	brew install meson

run_filter:
	cd CustomFilter &&\
	meson builddir &&\
	ninja -C builddir
	gst-launch-1.0  --gst-plugin-path=CustomFilter/builddir audiotestsrc wave=sine num-buffers=100 ! audioconvert ! customfilter ! faac ! mpegtsmux ! fdsink | ffplay -autoexit -i -
	ffmpeg -i http://touhouradio.com:8000/.mp3 -t 00:05 -c:a aac -f mpegts  -  | \
		gst-launch-1.0  --gst-plugin-path=CustomFilter/builddir \
		fdsrc ! tsparse ! tsdemux ! aacparse ! faad ! \
		audioresample ! audioconvert ! customfilter ! \
		faac ! mpegtsmux ! fdsink | \
		ffplay -autoexit -i -