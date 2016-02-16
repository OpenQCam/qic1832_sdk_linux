***************SDK SOURCE CODE STRUCTURE****************************************

This qic1832_sdk directory structure is as below.

qic_api/qic_src/        => qic control API
       /qic_include/    => incldue file of qic control API
qic_example/	        => example code for capturing video streams, firmware update, camera control, etc.

***************VIEW CODE WITH QTCREATOR*****************************************

The following command can be used to execute Qtcreator and load qic1832_sdk source code automatically.

#qtcreator Makefile.am

Note that the version of Qtcreator should be 2.8.1 or later and the plugin below must be enabled.
Check "About Plugins"->"AutotoolsProjectManager", and enable it.


***************BUILD INSTRUCTIONS***********************************************

To build the library and sample code, please follow the steps below:

#cd qic_example
#./configure
#make

Then, the files below are created if the process successes,
(1)libqic.a: qic1822 api library.
(2)avc_only: To capture one avc video stream.
(3)avc_simulcast: To capture avc simulcast stream.
(4)avc_two_way: To capture YUYV/MJPEG raw stream and avc video stream simultaneously.
(5)example_control: To set/get QIC miscellaneous control through XU.
(6)example_fw_check: To check firmware information.
(7)example_fw_update: To update firmware.
(8)mjpeg_only: To capture one MJPEG video stream.
(9)vp8_simulcast: To capture vp8 simulcast stream.
(10)vp8_two_way: To capture YUV/MJPEG raw stream and VP8 video stream simultaneously.
(11)yuv_only: To capture one YUV raw stream.


***************RUN EXAMPLE CODE*************************************************
------------
How to capture avc simulcast stream by (2)avc_only
------------
1. Please run as root with the command below.
#sudo ./avc_only -h for setting usage

Options:
-o | --output		H.264/AVC output [filename]
-n | --device node  H.264/AVC device [devicenode]
-s | --AVCsize		H.264/AVC stream [width]x[height]
-f | --fps		    Framerate
-b | --bitrate	    Bitrate
-g | --gop		    GOP value
-c | --count		Capture Counter
-d | --demux		H264 bad frame check
-h | --help		    Print this message

2. How to dump avc encoding stream to a file.
#sudo ./avc_only -o test   (test is file name)

and then you can see test.264 file in the current folder.


------------
How to control QIC by (5)example_control
------------
1. The available command and its format can be listed by the command below.
#sudo ./example_control -h

2. Set the device node and make sure the one is correct. This command also list controls.
e.g. #sudo ./example_control -d 1 (It may be 0,1,2...)

3. Remember to set the device node before adding other commands.
e.g. To reset device with #sudo ./example_control -d 1 -r
 
4. Type the control id you want to set/get value.
e.g.
#sudo ./example_control -d 1 -g 26
#sudo ./example_control -d 1 -s "26 2"


------------
How to update firmware by (7)example_fw_update
------------
1.
#sudo ./example_fw_update [options]

Options:
-r | --readsys 	 read system f/w version
-R | --readfile 	read file f/w version
-b | --bdir 	 set backup dir
-f | --usbbin 	 USB firmware file name
-i | --ispbin 	 ISP Parameter file name
-s | --BL2ndbin 	 2nd Boot Rom file name
-a | --audiobin 	 audio calibration data file name
-o | --osdfontbin 	 osd font file name
-h | --help	 Print this message

2. (Optional)Backup firmware image to current path.
#sudo ./example_fw_update -b ./

Then the following files will be dumped from flash.
QIC1822A_USB.bin(main firmware image),
QIC1822A_ISP_LSC_XXX.bin(camera ISP parameter image), and
QIC1822A_2ndBL.bin(2nd boot rom)(If the firmware supports it).

3. Update firmware version by specified firmware binary files.
#sudo ./example_fw_update -f ./QIC1822A_USB.bin -i ./QIC1822A_ISP_LSC_XXX.bin

The firmware update completes here.
And the following steps are the options for checking device/file firmware version.

4. Check camera firmware version.
#sudo ./example_fw_update -r

5. Check firmware binary file version.
#sudo ./example_fw_update -R -f ./QIC1822A_USB.bin

[Important Note]
1. If device boots from QIC1822 Boot rom, it's necessary to download QIC1822A_2ndBL.bin(2nd boot rom code) before download QIC1822A_USB.bin(main firmware image)

2. While the camera flash has QIC1822A_2ndBL.bin(2nd boot rom code), we don't suggest to update it.
We suggest to update firmware QIC1822A_USB.bin(main firmware image) and QIC1822A_ISP_LSC_XXX.bin(camera sensor tuning vlaue) only as below command.
#sudo ./example_fw_update -f ./QIC1822A_USB.bin -i ./QIC1822A_ISP_LSC_XXX.bin

