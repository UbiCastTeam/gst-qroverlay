## Project information

This gstreamer plugin will overlay a qrcode over a video stream that contains similar information as timeoverlay (buffer timestamps), the element name, as well as additional custom information. By running it on a display and capturing the display by e.g. filming it, it can be used to check how a capture device behaves and skips frames automatically.

## Usage

	gst-launch-1.0 videotestsrc pattern=2 num-buffers=600 ! "video/x-raw, format=(string)I420, width=(int)320, height=(int)240, framerate=(fraction)60000/1001" ! qroverlay name=toto silent=true x=50 qrcode-error-correction=3 extra-data-span-buffers=1 extra-data-interval-buffers=30 extra-data-name=trick extra-data-array="240,480,720,960,1200,1440,1680,1920" size=2 ! videoconvert ! xvimagesink


	gst-launch-1.0 videotestsrc pattern=2 num-buffers=600 ! "video/x-raw, format=(string)I420, width=(int)320, height=(int)240, framerate=(fraction)60000/1001" ! qroverlay name=toto silent=true x=50 qrcode-error-correction=3 extra-data-span-buffers=1 extra-data-interval-buffers=30 extra-data-name=trick extra-data-array="240,480,720,960,1200,1440,1680,1920" size=2 ! x264enc ! h264parse ! matroskamux ! filesink location=test_qrcode.mkv

## Installation and requirements

License: GPLv2 +

Dependencies:

	* libqrencode-dev
	* libgstreamer1.0-dev
	* libgstreamer-plugins-base1.0-dev
