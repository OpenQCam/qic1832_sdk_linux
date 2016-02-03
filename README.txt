***************SDK SOURCE CODE STRUCTURE****************************************

This qic1832_sdk directory structure is:

qic_api/qic_src/        => qic control API
       /qic_include/    => incldue file of qic control API
qic_example/	        => example code for capturing video streams, firmware update, etc.

***************VIEW CODE WITH QTCREATOR*****************************************

The following command can be used to execute Qtcreator and automatically load qic1832_sdk source code.

#qtcreator Makefile.am

Not that the Qtcreator shoulde be 2.8.1 or later and enable the plugin below.
Check "About Plugins"->"AutotoolsProjectManager", and enable it.


***************BUILD INSTRUCTIONS***********************************************

To build the library and sample code, please follow the steps below:

#cd qic_example
#./configure
#make

You will get the files below if build successes,
(1)libqic.a: qic1822 api library.
(2)avc_only: To capture one avc video stream.
(3)avc_simulcast: To capture avc simulcast stream.
(4)avc_two_way: To capture YUYV/MJPEG raw stream and avc video stream simultaneously.
(5)example_control: To set/get QIC miscellaneous control through XU.
(6)example_fw_check: To check firmware information.
(7)example_fw_update: To update firmware.
(8)mjpeg_only: To capture one MJPEG video stream.
(9)avc_only: To capture one avc video stream.
(10)vp8_simulcast: To capture vp8 simulcast stream.
(11)vp8_two_way: To capture YUV/MJPEG raw stream and VP8 video stream simultaneously.
(12)yuv_only: To capture one YUV raw stream.

 
***************RUN EXAMPLE CODE*************************************************

1. Please run as root.

2. How to run vp8_only for capture vp8 video stream.
#sudo ./vp8_only -h for setting usage

Options:
-o | --output		VP8 output [filename]
-s | --VP8size		VP8 stream [width]x[height]
-y | --YUVsize		YUV raw stream [width]x[height]
-f | --fps		    Framerate
-b | --bitrate		bitrate
-g | --gop		    GOP value
-c | --count		Capture Counter
-h | --help		    Print this message

3. How to dump vp8 encoding stream to a file.
#sudo ./vp8_only -o test   (test is file name)

and you can see the test.ivf file on current folder.

4. How to update firmware by example_fw_update
#sudo ./ -h
Usage: ./example_fw_update [options]

options:
-r | --readsys 	    read system f/w version
-R | --readfile     read file f/w version
-b | --bdir 	    set backup dir
-f | --usbbin 	    USB firmware file name
-i | --ispbin 	    ISP Parameter file name
-s | --ispbin 	    2ns Boot Rom file name
-h | --help	 Print this message

4-1. Backup firmware image to current path
#sudo ./example_fw_update -b ./

and you can see QIC1822A_USB.bin(main firmware image) and QIC1822A_ISP_LSC_XXX.bin(camera sensor tuning vlaue) firmware binary
(and QIC1822A_2ndBL.bin (2nd boot rom code) files) that dump from camera.

4-2. Update firmware version by specified firmware binary files
#sudo ./example_fw_update -s ./QIC1822_2nd_BL.bin -f ./QIC1822A_USB.bin -i ./QIC1822A_ISP_LSC_XXX.bin

4-3. Check camera firmware version
#sudo ./example_fw_update -r

4-4. Check firmware binary file version
#sudo ./example_fw_update -R -f ./QIC1822A_USB.bin

[Important Note]
1. It is necessary to download QIC1822A_2ndBL.bin (2nd boot rom code) before download QIC1822A_USB.bin(main firmware image) while device boots from QIC1822 Boot rom.

2. While camera flash exists QIC1822A_2ndBL.bin (2nd boot rom code), we don't suggest to update it.
We suggest to update firmware QIC1822A_USB.bin(main firmware image) and QIC1822A_ISP_LSC_XXX.bin(camera sensor tuning vlaue) only as below command.
#sudo ./example_fw_update -f ./QIC1822A_USB.bin -i ./QIC1822A_ISP_LSC_XXX.bin

