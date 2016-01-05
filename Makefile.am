AUTOMAKE_OPTIONS= foreign

lib_LIBRARIES = libqic_sky_module.a libqic_sky_module_release.a

libqic_sky_module_a_SOURCES = qic_api/qic_src/H264Enum.c qic_api/qic_src/demux.c qic_api/qic_src/qic_control.c qic_api/qic_src/qic_xuctrl.c qic_api/qic_include/H264Enum.h qic_api/qic_include/demux.h qic_api/qic_include/qic_control.h qic_api/qic_include/qic_xuctrl.h  qic_api/qic_include/qic_cfg.h qic_api/qic_include/qic_firmware.h  
libqic_sky_module_a_SOURCES +=qic_api/qic_src/vp8demux.c qic_api/qic_include/vp8demux.h qic_api/qic_src/mdparse.c qic_api/qic_include/mdparse.h
libqic_sky_module_a_CFLAGS = -O2 -g -Wall
libqic_sky_module_a_CFLAGS += -DDEBUG_LOG

libqic_sky_module_release_a_SOURCES = qic_api/qic_src/H264Enum.c qic_api/qic_src/demux.c qic_api/qic_src/qic_control.c qic_api/qic_src/qic_xuctrl.c qic_api/qic_include/H264Enum.h qic_api/qic_include/demux.h qic_api/qic_include/qic_control.h qic_api/qic_include/qic_xuctrl.h  qic_api/qic_include/qic_cfg.h qic_api/qic_include/qic_firmware.h  
libqic_sky_module_release_a_SOURCES +=qic_api/qic_src/vp8demux.c qic_api/qic_include/vp8demux.h
libqic_sky_module_release_a_CFLAGS = -O2 -g -Wall 
libqic_sky_module_release_a_CFLAGS += -DVP8API

bin_PROGRAMS= bin/avc_two_way bin/example_yuv bin/example_mjpg bin/example_avc bin/fw_downloader bin/fw_check 
bin_PROGRAMS+=bin/example_vp8 bin/vp8_two_way
bin_PROGRAMS+=bin/avc_simulcast bin/vp8_simulcast
bin_PROGRAMS+=bin/example_control

bin_avc_two_way_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include 
bin_avc_two_way_CFLAGS += -DUSE_FWRITE  
bin_avc_two_way_LDFLAGS = -L./ 
bin_avc_two_way_LDADD = -lrt -lqic_sky_module
bin_avc_two_way_SOURCES = qic_example/avc_two_way.c qic_example/misc_writefile.c qic_example/misc_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_example_yuv_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include
bin_example_yuv_CFLAGS += -DUSE_FWRITE
bin_example_yuv_LDFLAGS = -L./
bin_example_yuv_LDADD = -lrt -lqic_sky_module
bin_example_yuv_SOURCES = qic_example/example_yuvonly.c qic_example/misc_writefile.c qic_example/misc_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_example_mjpg_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include 
bin_example_mjpg_CFLAGS += -DUSE_FWRITE
bin_example_mjpg_LDFLAGS = -L./ 
bin_example_mjpg_LDADD = -lrt -lqic_sky_module
bin_example_mjpg_SOURCES = qic_example/example_mjpgonly.c qic_example/misc_writefile.c qic_example/misc_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_example_avc_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include
bin_example_avc_CFLAGS += -DUSE_FWRITE
bin_example_avc_LDFLAGS = -L./
bin_example_avc_LDADD = -lrt -lqic_sky_module 
bin_example_avc_SOURCES = qic_example/example_avconly.c qic_example/misc_writefile.c qic_example/misc_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_example_vp8_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include 
bin_example_vp8_CFLAGS += -DUSE_FWRITE
bin_example_vp8_LDFLAGS = -L./ 
bin_example_vp8_LDADD = -lrt -lqic_sky_module
bin_example_vp8_SOURCES = qic_example/example_vp8only.c qic_example/misc_writefile.c qic_example/misc_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_fw_downloader_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include
bin_fw_downloader_CFLAGS += -DUSE_FWRITE
bin_fw_downloader_LDFLAGS = -L./
bin_fw_downloader_LDADD = -lrt -lqic_sky_module
bin_fw_downloader_SOURCES = qic_example/fw_downloader.c qic_example/misc_system.c qic_example/misc_system.h

bin_fw_check_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include
bin_fw_check_CFLAGS += -DUSE_FWRITE
bin_fw_check_LDFLAGS = -L./
bin_fw_check_LDADD = -lrt -lqic_sky_module
bin_fw_check_SOURCES = qic_example/fw_check.c qic_example/misc_system.c qic_example/misc_system.h

bin_vp8_two_way_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include -Iqic_api/sky_include
bin_vp8_two_way_CFLAGS += -DUSE_FWRITE
bin_vp8_two_way_LDFLAGS = -L./ 
bin_vp8_two_way_LDADD = -lrt -lqic_sky_module
bin_vp8_two_way_SOURCES = qic_example/vp8_two_way.c qic_example/misc_writefile.c qic_example/misc_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_vp8_simulcast_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include 
bin_vp8_simulcast_CFLAGS += -DUSE_FWRITE
bin_vp8_simulcast_LDFLAGS = -L./ 
bin_vp8_simulcast_LDADD = -lrt -lqic_sky_module
bin_vp8_simulcast_SOURCES = qic_example/vp8_simulcast.c qic_example/simulcast_writefile.c qic_example/simulcast_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_avc_simulcast_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include 
bin_avc_simulcast_CFLAGS += -DUSE_FWRITE
bin_avc_simulcast_LDFLAGS = -L./ 
bin_avc_simulcast_LDADD = -lrt -lqic_sky_module
bin_avc_simulcast_SOURCES = qic_example/avc_simulcast.c qic_example/simulcast_writefile.c qic_example/simulcast_writefile.h qic_example/misc_system.c qic_example/misc_system.h

bin_example_control_CFLAGS = -g -Wall -O2 -Iqic_api/qic_include
bin_example_control_CFLAGS += -DUSE_FWRITE
bin_example_control_LDFLAGS = -L./
bin_example_control_LDADD = -lrt -lqic_sky_module
bin_example_control_SOURCES = qic_example/example_control.c qic_example/misc_system.c qic_example/misc_system.h
