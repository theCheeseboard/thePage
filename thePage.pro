TEMPLATE = subdirs

libproj.subdir = libthepage

applicationproj.subdir = application
applicationproj.depends = libproj

pluginsproj.subdir = plugins
pluginsproj.depends = libproj

SUBDIRS += \
    libproj \
    applicationproj \
    pluginsproj
