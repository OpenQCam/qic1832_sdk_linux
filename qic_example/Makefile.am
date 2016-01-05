AUTOMAKE_OPTIONS= foreign

lib_LIBRARIES = libqic_sky_module.a libqic_sky_module_release.a

libqic_sky_module_a_SOURCES = ../qic_api/qic_src/H264Enum.c ../qic_api/qic_src/demux.c ../qic_api/qic_src/qic_control.c ../qic_api/qic_src/qic_xuctrl.c ../qic_api/qic_include/H264Enum.h ../qic_api/qic_include/demux.h ../qic_api/qic_include/qic_control.h ../qic_api/qic_include/qic_xuctrl.h  ../qic_api/qic_include/qic_cfg.h ../qic_api/qic_include/qic_firmware.h  
libqic_sky_module_a_SOURCES +=../qic_api/qic_src/vp8demux.c ../qic_api/qic_include/vp8demux.h ../qic_api/qic_src/mdparse.c ../qic_api/qic_include/mdparse.h
libqic_sky_module_a_CFLAGS = -O2 -g -Wall
libqic_sky_module_a_CFLAGS += -DDEBUG_LOG

libqic_sky_module_release_a_SOURCES = ../qic_api/qic_src/H264Enum.c ../qic_api/qic_src/demux.c ../qic_api/qic_src/qic_control.c ../qic_api/qic_src/qic_xuctrl.c ../qic_api/qic_include/H264Enum.h ../qic_api/qic_include/demux.h ../qic_api/qic_include/qic_control.h ../qic_api/qic_include/qic_xuctrl.h  ../qic_api/qic_include/qic_cfg.h ../qic_api/qic_include/qic_firmware.h  
libqic_sky_module_release_a_SOURCES +=../qic_api/qic_src/vp8demux.c ../qic_api/qic_include/vp8demux.h
libqic_sky_module_release_a_CFLAGS = -O2 -g -Wall 
libqic_sky_module_release_a_CFLAGS += -DVP8API

bin_PROGRAMS= avc_two_way example_yuv example_mjpg example_avc fw_downloader fw_check 
bin_PROGRAMS+=example_vp8 vp8_two_way
bin_PROGRAMS+=avc_simulcast vp8_simulcast
bin_PROGRAMS+=example_control

avc_two_way_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
avc_two_way_CFLAGS += -DUSE_FWRITE  
avc_two_way_LDFLAGS = -L./ 
avc_two_way_LDADD = -lrt -lqic_sky_module
avc_two_way_SOURCES =avc_two_way.c misc_writefile.c misc_writefile.h misc_system.c misc_system.h

example_yuv_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
example_yuv_CFLAGS += -DUSE_FWRITE
example_yuv_LDFLAGS = -L./
example_yuv_LDADD = -lrt -lqic_sky_module
example_yuv_SOURCES = example_yuvonly.c misc_writefile.c misc_writefile.h misc_system.c misc_system.h

example_mjpg_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
example_mjpg_CFLAGS += -DUSE_FWRITE
example_mjpg_LDFLAGS = -L./ 
example_mjpg_LDADD = -lrt -lqic_sky_module
example_mjpg_SOURCES = example_mjpgonly.c misc_writefile.c misc_writefile.h misc_system.c misc_system.h

example_avc_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
example_avc_CFLAGS += -DUSE_FWRITE
example_avc_LDFLAGS = -L./
example_avc_LDADD = -lrt -lqic_sky_module 
example_avc_SOURCES = example_avconly.c misc_writefile.c misc_writefile.h misc_system.c misc_system.h

example_vp8_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
example_vp8_CFLAGS += -DUSE_FWRITE
example_vp8_LDFLAGS = -L./ 
example_vp8_LDADD = -lrt -lqic_sky_module
example_vp8_SOURCES = example_vp8only.c misc_writefile.c misc_writefile.h misc_system.c misc_system.h

fw_downloader_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
fw_downloader_CFLAGS += -DUSE_FWRITE
fw_downloader_LDFLAGS = -L./
fw_downloader_LDADD = -lrt -lqic_sky_module
fw_downloader_SOURCES = fw_downloader.c misc_system.c misc_system.h

fw_check_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
fw_check_CFLAGS += -DUSE_FWRITE
fw_check_LDFLAGS = -L./
fw_check_LDADD = -lrt -lqic_sky_module
fw_check_SOURCES = fw_check.c misc_system.c misc_system.h

vp8_two_way_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include -I../qic_api/sky_include
vp8_two_way_CFLAGS += -DUSE_FWRITE
vp8_two_way_LDFLAGS = -L./ 
vp8_two_way_LDADD = -lrt -lqic_sky_module
vp8_two_way_SOURCES = vp8_two_way.c misc_writefile.c misc_writefile.h misc_system.c misc_system.h

vp8_simulcast_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
vp8_simulcast_CFLAGS += -DUSE_FWRITE
vp8_simulcast_LDFLAGS = -L./ 
vp8_simulcast_LDADD = -lrt -lqic_sky_module
vp8_simulcast_SOURCES = vp8_simulcast.c simulcast_writefile.c simulcast_writefile.h misc_system.c misc_system.h

avc_simulcast_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
avc_simulcast_CFLAGS += -DUSE_FWRITE
avc_simulcast_LDFLAGS = -L./ 
avc_simulcast_LDADD = -lrt -lqic_sky_module
avc_simulcast_SOURCES = avc_simulcast.c simulcast_writefile.c simulcast_writefile.h misc_system.c misc_system.h

example_control_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
example_control_CFLAGS += -DUSE_FWRITE
example_control_LDFLAGS = -L./
example_control_LDADD = -lrt -lqic_sky_module
example_control_SOURCES = example_control.c misc_system.c misc_system.h

exampledir=qic_example
distdir=$(PACKAGE)-$(VERSION)/$(exampledir)
tardir1=$(PACKAGE)-$(VERSION)
EXTRA_DIST =../*.txt ./libbuild.sh

am__remove_qicdistdir = \
  { test ! -d "$(tardir1)" \
    || { find "$(tardir1)" -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr "$(tardir1)"; }; }

qicdist:
	test -d "$(PACKAGE)-$(VERSION)" || mkdir "$(PACKAGE)-$(VERSION)"
	test -d "$(distdir)" || mkdir "$(distdir)"
	@srcdirstrip=`echo "$(srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	topsrcdirstrip=`echo "$(top_srcdir)" | sed 's/[].[^$$\\*]/\\\\&/g'`; \
	list='$(DISTFILES)'; \
	  dist_files=`for file in $$list; do echo $$file; done | \
	  sed -e "s|^$$srcdirstrip/||;t" \
	      -e "s|^$$topsrcdirstrip/|$(top_builddir)/|;t"`; \
	case $$dist_files in \
	  */*)  $(MKDIR_P) `echo "$$dist_files" | \
			   sed '/\//!d;s|^|$(distdir)/|;s,/[^/]*$$,,' | \
			   sort -u` ;; \
	esac; \
	for file in $$dist_files; do \
	  if test -f $$file || test -d $$file; then d=.; else d=$(srcdir); fi; \
	  if test -d $$d/$$file; then \
	    dir=`echo "/$$file" | sed -e 's,/[^/]*$$,,'`; \
	    if test -d "$(distdir)/$$file"; then \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    if test -d $(srcdir)/$$file && test $$d != $(srcdir); then \
	      cp -fpR $(srcdir)/$$file "$(distdir)$$dir" || exit 1; \
	      find "$(distdir)/$$file" -type d ! -perm -700 -exec chmod u+rwx {} \;; \
	    fi; \
	    cp -fpR $$d/$$file "$(distdir)$$dir" || exit 1; \
	  else \
	    test -f "$(distdir)/$$file" \
	    || cp -p $$d/$$file "$(distdir)/$$file" \
	    || exit 1; \
	  fi; \
	done
	-test -n "$(am__skip_mode_fix)" \
	|| find "$(distdir)" -type d ! -perm -755 \
		-exec chmod u+rwx,go+rx {} \; -o \
	  ! -type d ! -perm -444 -links 1 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -400 -exec chmod a+r {} \; -o \
	  ! -type d ! -perm -444 -exec $(install_sh) -c -m a+r {} {} \; \
	|| chmod -R a+r "$(distdir)"
	tardir=$(tardir1) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(tardir1).tar.gz
	$(am__remove_qicdistdir)


