******************SDK source code construct*************************************

The qic1822_sdk_relese_rxxx directory structure as follows:

qic_api/qic_src/           ==> qic control API
       /qic_include/
       /sky_src/           ==> skype control API
       /sky_include/	
qic_example/	           ==> example code for capture video streams and firmware upgrade.



******************BUILD INSTRUCTIONS*******************************************

To build the library and sample code, please following below step:

#cd qic_example
#./configure
#make

you will get below files if build success
(1)libqic_sky_module.a ==> qic1822 api library
(2)example_yuv  ==> example code to capture one YUV raw stream
(3)example_mjpeg  ==> example code to capture one MJPEG video stream
(4)example_vp8  ==> example code to capture one VP8 video stream	  
(5)vp8_two_way ==> example code to capture YUV/MJPEG raw stream and VP8 video stream simultaneously.
(6)fw_downloader ==> example code to update firmware
(7)avc_two_way ==> example code to capture YUYV/MJPEG raw stream and AVC video stream simultaneously.
(8)avc_simulcast ==> example to capture avc simulcast stream.
(9)vp8_simulcast ==> example to capture vp8 simulcast stream.
 
******************Runing example code *******************************************
1. to run example code please run as root.

2. how to run example_vp8 for capture vp8 vieo stream.
#sudo ./example_vp8 -h for setting usage

Options:
-o | --output		VP8 output [filename]
-s | --VP8size		VP8 stream [width]x[height]
-y | --YUVsize		YUV raw stream [width]x[height]
-f | --fps		Framerate
-b | --bitrate		bitrate
-g | --gop		GOP value
-c | --count		Capture Counter
-h | --help		Print this message

3. how to dump vp8 encoding stream to a file.
#sudo ./example_vp8 -o test   (test is file name) 

and you can see the test.ivf file on current folder.

4. how to update firmware by fw_downloader
#sudo ./fw_downloader -h
Usage: ./fw_downloader [options]

Options:
-r | --readsys 	 read system f/w version
-R | --readfile 	read file f/w version
-b | --bdir 	 set backup dir
-f | --usbbin 	 USB firmware file name
-i | --ispbin 	 ISP Parameter file name
-s | --ispbin 	 2ns Boot Rom file name
-h | --help	 Print this message


4-1. backup firmware image to current path
#sudo ./fw_downloader -b ./   

and you can see QIC1822A_USB.bin(main firmware image) and QIC1822A_ISP_LSC_XXX.bin(camera sensor tuning vlaue) firmware binary   and QIC1822A_2ndBL.bin (2nd boot rom code) files that dump from camera.

4-2. update firmware version by specify firmware binary files
#sudo ./fw_downloader -s ./QIC1822_2nd_BL.bin -f ./QIC1822A_USB.bin -i ./QIC1822A_ISP_LSC_XXX.bin

4-3. check camera firmware version
#sudo ./fw_downloader -r

4-4. check firmware binary file version
#sudo ./fw_downloader -R -f ./QIC1822A_USB.bin

[Important Note]
1. It is necessary to download QIC1822A_2ndBL.bin (2nd boot rom code) before download QIC1822A_USB.bin(main firmware image). while you are boot from QIC1822 Boot rom.

2. While camera flash exist QIC1822A_2ndBL.bin (2nd boot rom code), we don't suggest to upgrade QIC1822A_2ndBL.bin (2nd boot rom code).  we suggest to upgrade firmware QIC1822A_USB.bin(main firmware image) and QIC1822A_ISP_LSC_XXX.bin(camera sensor tuning vlaue) only. as below command.
#sudo ./fw_downloader -f ./QIC1822A_USB.bin -i ./QIC1822A_ISP_LSC_XXX.bin







