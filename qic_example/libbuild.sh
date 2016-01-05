#!/bin/sh

QIC_RELEASE_PATH=qic1822_sample


if [ -d $QIC_RELEASE_PATH ]
then
    echo "remove $QIC_RELEASE_PATH!!"
rm -fR $QIC_RELEASE_PATH
fi

if [ -f $QIC_RELEASE_PATH.tar.gz ]
then
    echo "remove $QIC_RELEASE_PATH.tar.gz!!"
rm -fR $QIC_RELEASE_PATH.tar.gz
fi


test 
mkdir $QIC_RELEASE_PATH
mkdir $QIC_RELEASE_PATH/qic
mkdir $QIC_RELEASE_PATH/qic/include
mkdir $QIC_RELEASE_PATH/qic/lib
cp ../qic_api/qic_include/* $QIC_RELEASE_PATH/qic/include
cp libqic_sky_module_release.a $QIC_RELEASE_PATH/qic/lib/libqic_module.a
cp fw_downloader.c $QIC_RELEASE_PATH
cp fw_check.c $QIC_RELEASE_PATH

cd $QIC_RELEASE_PATH

touch Makefile


echo "ifndef verbose" >>Makefile
echo "SILENT = @ " >>Makefile
echo "endif" >>Makefile

echo "ifndef CC" >>Makefile
echo "  CC = gcc" >>Makefile
echo "endif" >>Makefile

echo "ifndef CXX" >>Makefile
echo "  CXX = g++" >>Makefile
echo "endif" >>Makefile

echo "ifndef AR" >>Makefile
echo "  AR = ar" >>Makefile
echo "endif" >>Makefile

echo "TARGETDIR  =" >>Makefile

echo "INCLUDES   = -I./qic/include" >>Makefile
echo "DOWNLOADER_TARGET     = \$(TARGETDIR)fw_downloader" >>Makefile  
echo "CHECK_TARGET     = \$(TARGETDIR)fw_check" >>Makefile
echo "DEFINES    += -D_DEBUG -DDEBUG" >>Makefile
echo "CPPFLAGS   += -MMD -MP \$(DEFINES) \$(INCLUDES)" >>Makefile
echo "CFLAGS     += \$(CPPFLAGS) \$(ARCH) -Wall -c " >>Makefile
echo "CXXFLAGS   += \$(CFLAGS) -fno-rtti -fno-exceptions" >>Makefile
echo "LDFLAGS    += -L./ -L./qic/lib" >>Makefile
echo "LIBS      += -lqic_module -lrt" >>Makefile 
echo "RESFLAGS  += \$(DEFINES) \$(INCLUDES)" >>Makefile
echo "LDDEPS    +=" >>Makefile
echo "SRC	   = fw_downloader.c" >>Makefile
echo "OBJECTS	   = fw_downloader.o" >>Makefile

echo "DOWNLOADER_SRC	  = fw_downloader.c" >>Makefile
echo "DOWNLOADER_OBJECTS  = fw_downloader.o" >>Makefile
echo "CHECK_SRC	          = fw_check.c" >>Makefile
echo "CHECK_OBJECTS	  = fw_check.o" >>Makefile


echo "all: \$(DOWNLOADER_TARGET) \$(CHECK_TARGET)" >>Makefile

echo "fw_downloader:" >>Makefile
echo "	\$(CC) -c \$(INCLUDES) \$(DOWNLOADER_SRC)" >>Makefile
echo "	\$(CC) -o \$(DOWNLOADER_TARGET) \$(DOWNLOADER_OBJECTS) \$(LDFLAGS) \$(LIBS)" >>Makefile

echo "fw_check:" >>Makefile
echo "	\$(CC) -c \$(INCLUDES) \$(CHECK_SRC)" >>Makefile
echo "	\$(CC) -o \$(CHECK_TARGET) \$(CHECK_OBJECTS) \$(LDFLAGS) \$(LIBS)" >>Makefile
 
echo "clean:" >>Makefile
echo "	rm -f \$(DOWNLOADER_OBJECTS) \$(CHECK_OBJECTS) \$(DOWNLOADER_TARGET) \$(CHECK_TARGET)" >>Makefile



cd ..

if [ -d $QIC_RELEASE_PATH ]
then
    echo "found $QIC_RELEASE_PATH!!"
chmod -R 777 $QIC_RELEASE_PATH
tar zcvf $QIC_RELEASE_PATH.tar.gz $QIC_RELEASE_PATH

rm -fR $QIC_RELEASE_PATH

fi




