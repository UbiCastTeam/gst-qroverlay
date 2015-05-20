## Documentation and tests


	gst-launch-1.0 videotestsrc pattern=2 num-buffers=600 ! "video/x-raw, format=(string)I420, width=(int)320, height=(int)240, framerate=(fraction)60000/1001" ! qroverlay name=toto silent=true x=50 qrcode-error-correction=3 extra-data-span-buffers=1 extra-data-interval-buffers=30 extra-data-name=trick extra-data-array="240,480,720,960,1200,1440,1680,1920" size=2 ! videoconvert ! xvimagesink


	gst-launch-1.0 videotestsrc pattern=2 num-buffers=600 ! "video/x-raw, format=(string)I420, width=(int)320, height=(int)240, framerate=(fraction)60000/1001" ! qroverlay name=toto silent=true x=50 qrcode-error-correction=3 extra-data-span-buffers=1 extra-data-interval-buffers=30 extra-data-name=trick extra-data-array="240,480,720,960,1200,1440,1680,1920" size=2 ! x264enc ! h264parse ! matroskamux ! filesink location=test_qrcode.mkv

## Installation and requirements

License: GPLv2 +

Dependencies:

	* libqrencode-dev
	* libgstreamer1.0-dev
	* libgstreamer-plugins-base1.0-dev
