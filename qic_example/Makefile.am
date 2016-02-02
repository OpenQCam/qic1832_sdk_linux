AUTOMAKE_OPTIONS= foreign

lib_LIBRARIES = libqic.a libqic_release.a

libqic_a_SOURCES = ../qic_api/qic_include/qic_cfg.h ../qic_api/qic_include/qic_firmware.h
libqic_a_SOURCES +=../qic_api/qic_src/demux_h264.c ../qic_api/qic_include/demux_h264.h
libqic_a_SOURCES +=../qic_api/qic_src/demux_vp8.c ../qic_api/qic_include/demux_vp8.h   
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_common_define.c ../qic_api/qic_include/qic_api_common_define.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_fw_update.c ../qic_api/qic_include/qic_api_fw_update.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_md.c ../qic_api/qic_include/qic_api_md.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_misc.c ../qic_api/qic_include/qic_api_misc.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_osd.c ../qic_api/qic_include/qic_api_osd.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_stream.c ../qic_api/qic_include/qic_api_stream.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_v4l2_ctpu.c ../qic_api/qic_include/qic_api_v4l2_ctpu.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_api_video.c ../qic_api/qic_include/qic_api_video.h
libqic_a_SOURCES +=../qic_api/qic_src/qic_utility_for_demo.c ../qic_api/qic_include/qic_utility_for_demo.h  
libqic_a_SOURCES +=../qic_api/qic_src/qic_xuctrl.c ../qic_api/qic_include/qic_xuctrl.h
libqic_a_CFLAGS = -O2 -g -Wall
libqic_a_CFLAGS += -DDEBUG_LOG

libqic_release_a_SOURCES = ../qic_api/qic_include/qic_cfg.h
libqic_release_a_SOURCES +=../qic_api/qic_src/demux_h264.c ../qic_api/qic_include/demux_h264.h
libqic_release_a_SOURCES +=../qic_api/qic_src/demux_vp8.c ../qic_api/qic_include/demux_vp8.h   
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_common_define.c ../qic_api/qic_include/qic_api_common_define.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_fw_update.c ../qic_api/qic_include/qic_api_fw_update.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_md.c ../qic_api/qic_include/qic_api_md.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_misc.c ../qic_api/qic_include/qic_api_misc.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_osd.c ../qic_api/qic_include/qic_api_osd.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_stream.c ../qic_api/qic_include/qic_api_stream.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_v4l2_ctpu.c ../qic_api/qic_include/qic_api_v4l2_ctpu.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_api_video.c ../qic_api/qic_include/qic_api_video.h
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_utility_for_demo.c ../qic_api/qic_include/qic_utility_for_demo.h  
libqic_release_a_SOURCES +=../qic_api/qic_src/qic_xuctrl.c ../qic_api/qic_include/qic_xuctrl.h
libqic_release_a_CFLAGS = -O2 -g -Wall 
libqic_release_a_CFLAGS += -DVP8API

bin_PROGRAMS= yuv_only mjpg_only avc_only vp8_only 
bin_PROGRAMS+=avc_two_way vp8_two_way
bin_PROGRAMS+=avc_simulcast vp8_simulcast
bin_PROGRAMS+=example_control example_fw_update example_fw_check 

yuv_only_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
yuv_only_CFLAGS += -DUSE_FWRITE
yuv_only_LDFLAGS = -L./
yuv_only_LDADD = -lrt -lqic
yuv_only_SOURCES = yuv_only.c
yuv_only_SOURCES += misc_writefile.c misc_writefile.h

mjpg_only_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
mjpg_only_CFLAGS += -DUSE_FWRITE
mjpg_only_LDFLAGS = -L./ 
mjpg_only_LDADD = -lrt -lqic
mjpg_only_SOURCES = mjpg_only.c
mjpg_only_SOURCES += misc_writefile.c misc_writefile.h

avc_only_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
avc_only_CFLAGS += -DUSE_FWRITE
avc_only_LDFLAGS = -L./
avc_only_LDADD = -lrt -lqic
avc_only_SOURCES = avc_only.c
avc_only_SOURCES += misc_writefile.c misc_writefile.h

vp8_only_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
vp8_only_CFLAGS += -DUSE_FWRITE
vp8_only_LDFLAGS = -L./ 
vp8_only_LDADD = -lrt -lqic
vp8_only_SOURCES = vp8_only.c
vp8_only_SOURCES += misc_writefile.c misc_writefile.h

avc_two_way_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
avc_two_way_CFLAGS += -DUSE_FWRITE  
avc_two_way_LDFLAGS = -L./ 
avc_two_way_LDADD = -lrt -lqic
avc_two_way_SOURCES = avc_two_way.c
avc_two_way_SOURCES += misc_writefile.c misc_writefile.h

avc_simulcast_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
avc_simulcast_CFLAGS += -DUSE_FWRITE
avc_simulcast_LDFLAGS = -L./ 
avc_simulcast_LDADD = -lrt -lqic
avc_simulcast_SOURCES = avc_simulcast.c
avc_simulcast_SOURCES += misc_writefile_simulcast.c misc_writefile_simulcast.h

vp8_two_way_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include -I../qic_api/sky_include
vp8_two_way_CFLAGS += -DUSE_FWRITE
vp8_two_way_LDFLAGS = -L./ 
vp8_two_way_LDADD = -lrt -lqic
vp8_two_way_SOURCES = vp8_two_way.c
vp8_two_way_SOURCES += misc_writefile.c misc_writefile.h

vp8_simulcast_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include 
vp8_simulcast_CFLAGS += -DUSE_FWRITE
vp8_simulcast_LDFLAGS = -L./ 
vp8_simulcast_LDADD = -lrt -lqic
vp8_simulcast_SOURCES = vp8_simulcast.c
vp8_simulcast_SOURCES += misc_writefile_simulcast.c misc_writefile_simulcast.h

example_control_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
example_control_CFLAGS += -DUSE_FWRITE
example_control_LDFLAGS = -L./
example_control_LDADD = -lrt -lqic
example_control_SOURCES = example_control.c

example_fw_update_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
example_fw_update_CFLAGS += -DUSE_FWRITE
example_fw_update_LDFLAGS = -L./
example_fw_update_LDADD = -lrt -lqic
example_fw_update_SOURCES = example_fw_update.c

example_fw_check_CFLAGS = -g -Wall -O2 -I../qic_api/qic_include
example_fw_check_CFLAGS += -DUSE_FWRITE
example_fw_check_LDFLAGS = -L./
example_fw_check_LDADD = -lrt -lqic
example_fw_check_SOURCES = example_fw_check.c

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


