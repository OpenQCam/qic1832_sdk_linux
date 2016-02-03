AUTOMAKE_OPTIONS= foreign

lib_LIBRARIES = libqic.a libqic_release.a

libqic_a_SOURCES =\
qic_api/qic_include/qic_cfg.h qic_api/qic_include/qic_include_all.h\
qic_api/qic_src/demux_h264.c qic_api/qic_include/demux_h264.h\
qic_api/qic_src/demux_vp8.c qic_api/qic_include/demux_vp8.h\
qic_api/qic_src/qic_api_common_define.c qic_api/qic_include/qic_api_common_define.h\
qic_api/qic_src/qic_api_fw_update.c qic_api/qic_include/qic_api_fw_update.h\
qic_api/qic_src/qic_api_md.c qic_api/qic_include/qic_api_md.h\
qic_api/qic_src/qic_api_misc.c qic_api/qic_include/qic_api_misc.h\
qic_api/qic_src/qic_api_osd.c qic_api/qic_include/qic_api_osd.h\
qic_api/qic_src/qic_api_stream.c qic_api/qic_include/qic_api_stream.h\
qic_api/qic_src/qic_api_v4l2_ctpu.c qic_api/qic_include/qic_api_v4l2_ctpu.h\
qic_api/qic_src/qic_api_video.c qic_api/qic_include/qic_api_video.h\
qic_api/qic_src/qic_utility_for_demo.c qic_api/qic_include/qic_utility_for_demo.h\
qic_api/qic_src/qic_xuctrl.c qic_api/qic_include/qic_xuctrl.h
libqic_a_CFLAGS = -O2 -g -Wall
libqic_a_CFLAGS += -DDEBUG_LOG

libqic_release_a_SOURCES =\
qic_api/qic_include/qic_cfg.h qic_api/qic_include/qic_include_all.h\
qic_api/qic_src/demux_h264.c qic_api/qic_include/demux_h264.h\
qic_api/qic_src/demux_vp8.c qic_api/qic_include/demux_vp8.h\
qic_api/qic_src/qic_api_common_define.c qic_api/qic_include/qic_api_common_define.h\
qic_api/qic_src/qic_api_fw_update.c qic_api/qic_include/qic_api_fw_update.h\
qic_api/qic_src/qic_api_md.c qic_api/qic_include/qic_api_md.h\
qic_api/qic_src/qic_api_misc.c qic_api/qic_include/qic_api_misc.h\
qic_api/qic_src/qic_api_osd.c qic_api/qic_include/qic_api_osd.h\
qic_api/qic_src/qic_api_stream.c qic_api/qic_include/qic_api_stream.h\
qic_api/qic_src/qic_api_v4l2_ctpu.c qic_api/qic_include/qic_api_v4l2_ctpu.h\
qic_api/qic_src/qic_api_video.c qic_api/qic_include/qic_api_video.h\
qic_api/qic_src/qic_utility_for_demo.c qic_api/qic_include/qic_utility_for_demo.h\
qic_api/qic_src/qic_xuctrl.c qic_api/qic_include/qic_xuctrl.h
libqic_release_a_CFLAGS = -O2 -g -Wall 
libqic_release_a_CFLAGS += -DVP8API

bin_PROGRAMS= yuv_only mjpg_only avc_only vp8_only 
bin_PROGRAMS+=avc_two_way vp8_two_way
bin_PROGRAMS+=avc_simulcast vp8_simulcast
bin_PROGRAMS+=example_control example_fw_update example_fw_check 

yuv_only_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
yuv_only_CFLAGS += -DUSE_FWRITE
yuv_only_LDFLAGS = -L./
yuv_only_LDADD = -lrt -lqic
yuv_only_DEPENDENCIES = $(lib_LIBRARIES)
yuv_only_SOURCES = qic_example/yuv_only.c
yuv_only_SOURCES += qic_example/misc_writefile.c qic_example/misc_writefile.h

mjpg_only_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
mjpg_only_CFLAGS += -DUSE_FWRITE
mjpg_only_LDFLAGS = -L./
mjpg_only_LDADD = -lrt -lqic
mjpg_only_DEPENDENCIES = $(lib_LIBRARIES)
mjpg_only_SOURCES = qic_example/mjpg_only.c
mjpg_only_SOURCES += qic_example/misc_writefile.c qic_example/misc_writefile.h

avc_only_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
avc_only_CFLAGS += -DUSE_FWRITE
avc_only_LDFLAGS = -L./
avc_only_LDADD = -lrt -lqic
avc_only_DEPENDENCIES = $(lib_LIBRARIES)
avc_only_SOURCES = qic_example/avc_only.c
avc_only_SOURCES += qic_example/misc_writefile.c qic_example/misc_writefile.h

vp8_only_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
vp8_only_CFLAGS += -DUSE_FWRITE
vp8_only_LDFLAGS = -L./
vp8_only_LDADD = -lrt -lqic
vp8_only_DEPENDENCIES = $(lib_LIBRARIES)
vp8_only_SOURCES = qic_example/vp8_only.c
vp8_only_SOURCES += qic_example/misc_writefile.c qic_example/misc_writefile.h

avc_two_way_CFLAGS = -g -Wall -O2 -I qic_api/qic_include 
avc_two_way_CFLAGS += -DUSE_FWRITE
avc_two_way_LDFLAGS = -L./ 
avc_two_way_LDADD = -lrt -lqic
avc_two_way_DEPENDENCIES = $(lib_LIBRARIES)
avc_two_way_SOURCES = qic_example/avc_two_way.c
avc_two_way_SOURCES += qic_example/misc_writefile.c qic_example/misc_writefile.h

avc_simulcast_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
avc_simulcast_CFLAGS += -DUSE_FWRITE
avc_simulcast_LDFLAGS = -L./ 
avc_simulcast_LDADD = -lrt -lqic
avc_simulcast_DEPENDENCIES = $(lib_LIBRARIES)
avc_simulcast_SOURCES = qic_example/avc_simulcast.c
avc_simulcast_SOURCES += qic_example/misc_writefile_simulcast.c qic_example/misc_writefile_simulcast.h

vp8_two_way_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
vp8_two_way_CFLAGS += -DUSE_FWRITE
vp8_two_way_LDFLAGS = -L./
vp8_two_way_LDADD = -lrt -lqic
vp8_two_way_DEPENDENCIES = $(lib_LIBRARIES)
vp8_two_way_SOURCES = qic_example/vp8_two_way.c
vp8_two_way_SOURCES += qic_example/misc_writefile.c qic_example/misc_writefile.h

vp8_simulcast_CFLAGS = -g -Wall -O2 -I qic_api/qic_include 
vp8_simulcast_CFLAGS += -DUSE_FWRITE
vp8_simulcast_LDFLAGS = -L./ 
vp8_simulcast_LDADD = -lrt -lqic
vp8_simulcast_DEPENDENCIES = $(lib_LIBRARIES)
vp8_simulcast_SOURCES = qic_example/vp8_simulcast.c
vp8_simulcast_SOURCES += qic_example/misc_writefile_simulcast.c qic_example/misc_writefile_simulcast.h

example_control_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
example_control_CFLAGS += -DUSE_FWRITE
example_control_LDFLAGS = -L./
example_control_LDADD = -lrt -lqic
example_control_DEPENDENCIES = $(lib_LIBRARIES)
example_control_SOURCES = qic_example/example_control.c

example_fw_update_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
example_fw_update_CFLAGS += -DUSE_FWRITE
example_fw_update_LDFLAGS = -L./
example_fw_update_LDADD = -lrt -lqic
example_fw_update_DEPENDENCIES = $(lib_LIBRARIES)
example_fw_update_SOURCES = qic_example/example_fw_update.c

example_fw_check_CFLAGS = -g -Wall -O2 -I qic_api/qic_include
example_fw_check_CFLAGS += -DUSE_FWRITE
example_fw_check_LDFLAGS = -L./
example_fw_check_LDADD = -lrt -lqic
example_fw_check_DEPENDENCIES = $(lib_LIBRARIES)
example_fw_check_SOURCES = qic_example/example_fw_check.c

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


