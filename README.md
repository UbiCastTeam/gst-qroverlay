## Project information

This gstreamer plugin will overlay a qrcode over a video stream that contains buffer information, the element name, as well as additional custom information ("extra data"). By running it on a display and capturing the display by e.g. filming it, it can be used to analyze how a capture device behaves and skips frames.

The qrcode contains a json string with the following data:
```
{"timestamp":1151150000,"buffercount":70,"framerate":"60000/1001","name":"testoverlay",}
```

If you define extra data, the additional data will be inserted as well every "extra-data-interval-buffers", and spanned over "extra-data-span-buffers" consecutive buffers.
```
{"timestamp":0,"buffercount":1,"framerate":"60000/1001","name":"testoverlay","mydata":"240",}
```

With extra-data-interval-buffers=30 and extra-data-span-buffers=2, at 30 frames per second, buffers 1 and 2 will contain the extra data, then the extra data will be inserted again at buffers 60 and 61. Note that the extra data will cycle between values in a csv format string. This feature can be used to specify an audio frequency that is expected.


## Usage

```
gst-launch-1.0 videotestsrc pattern=2 num-buffers=600 ! "video/x-raw, format=(string)I420, width=(int)320, height=(int)240, framerate=(fraction)30/1" ! qroverlay name=testoverlay x=50 qrcode-error-correction=3 extra-data-span-buffers=1 extra-data-interval-buffers=30 extra-data-name=mydata extra-data-array="240,480,720,960,1200,1440,1680,1920" pixel-size=2 ! videoconvert ! xvimagesink
```



## Installation and requirements

License: GPLv2 +

Dependencies:

	* libqrencode-dev
	* libgstreamer1.0-dev
	* libgstreamer-plugins-base1.0-dev
