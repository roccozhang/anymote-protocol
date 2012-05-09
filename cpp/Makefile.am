## Process this file with automake to produce Makefile.in

# Make sure that when we re-make ./configure, we get the macros we need
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = .

# This is so we can #include <package/foo>
AM_CPPFLAGS = -I$(top_srcdir)/src \
  -I$(top_srcdir)/tests \
  -I$(GTEST_DIR) -I$(GTEST_DIR)/include \
  -I$(GMOCK_DIR) -I$(GMOCK_DIR)/include

# These are good warnings to turn on by default
if GCC
AM_CXXFLAGS = -Wall -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare
endif

# The -no-undefined flag allows libtool to generate shared libraries for
# Cygwin and MinGW.  LIBSTDCXX_LA_LINKER_FLAG is used to fix a Solaris bug.
# -version-info tells libtool what to put after the '.so' in the filename.
AM_LDFLAGS = -no-undefined $(LIBSTDCXX_LA_LINKER_FLAG) \
             -version-info @SO_VERSION@

# The libraries (.so's) to build and install.
lib_LTLIBRARIES = libanymote.la

# Libraries that are built but not installed.
noinst_LTLIBRARIES = libgtest.la libgmock.la

# The .h files you want to install (that is, .h files that people
# who install this package can include in their own applications.)
anymote_device_includedir = $(includedir)/anymote/device
anymote_device_include_HEADERS = \
  src/anymote/device/anymotelistener.h \
  src/anymote/device/devicesession.h

anymote_messages_includedir = $(includedir)/anymote/messages
anymote_messages_include_HEADERS = \
  src/anymote/messages/keycodes.pb.h \
  src/anymote/messages/messagelistener.h \
  src/anymote/messages/remote.pb.h

anymote_wire_includedir = $(includedir)/anymote/wire
anymote_wire_include_HEADERS = \
  src/anymote/wire/protobufwireadapter.h \
  src/anymote/wire/wireadapter.h \
  src/anymote/wire/wireinterface.h \
  src/anymote/wire/wirelistener.h

docdir = $(prefix)/share/doc/$(PACKAGE)-$(VERSION)
dist_doc_DATA = AUTHORS COPYING ChangeLog README

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = anymote.pc

## unittests to run when people type 'make check'.
TESTS = anymote-test
TESTS_ENVIRONMENT =
check_SCRIPTS =
# Every time you add a unittest to check_SCRIPTS, add it here too
noinst_SCRIPTS =

## vvvv RULES TO MAKE THE LIBRARIES, BINARIES, AND UNITTESTS

protoc_inputs = \
  ../proto/keycodes.proto \
  ../proto/remote.proto

protoc_outputs = \
  src/anymote/messages/keycodes.pb.cc \
  src/anymote/messages/keycodes.pb.h \
  src/anymote/messages/remote.pb.cc \
  src/anymote/messages/remote.pb.h

BUILT_SOURCES = $(protoc_outputs)

protoc_gen: $(protoc_inputs)
	$(PROTOC) --proto_path=../proto \
	    --cpp_out=$(srcdir)/src/anymote/messages \
            $(protoc_inputs)
	touch protoc_gen

$(protoc_outputs): protoc_gen

libanymote_la_CXXFLAGS = $(PROTOBUF_CFLAGS) $(GLOG_CFLAGS)
libanymote_la_LIBADD = $(PROTOBUF_LIBS) $(GLOG_LIBS)
libanymote_la_SOURCES = \
  src/anymote/device/devicesession.cc \
  src/anymote/messages/keycodes.pb.cc \
  src/anymote/messages/remote.pb.cc \
  src/anymote/wire/protobufwireadapter.cc

anymote_test_LDADD = libanymote.la libgtest.la libgmock.la

anymote_test_SOURCES = \
  tests/anymote/anymotetests.cc \
  tests/anymote/device/devicesessiontest.cc \
  tests/anymote/wire/protobufwireadaptertest.cc

libgtest_la_SOURCES = $(GTEST_DIR)/src/gtest-all.cc

libgmock_la_SOURCES = $(GMOCK_DIR)/src/gmock-all.cc

## ^^^^ END OF RULES TO MAKE THE LIBRARIES, BINARIES, AND UNITTESTS


## This should always include $(TESTS), but may also include other
## binaries that you compile but don't want automatically installed.
noinst_PROGRAMS = $(TESTS)

rpm: dist-gzip packages/rpm.sh packages/rpm/rpm.spec
	@cd packages && ./rpm.sh ${PACKAGE} ${VERSION}

deb: dist-gzip packages/deb.sh packages/deb/*
	@cd packages && ./deb.sh ${PACKAGE} ${VERSION}

# Windows wants write permission to .vcproj files and maybe even sln files.
dist-hook:
	test ! -e "$(distdir)/vsprojects" \
	   || chmod -R u+w $(distdir)/*.sln $(distdir)/vsprojects/

EXTRA_DIST = packages/rpm.sh packages/rpm/rpm.spec packages/deb.sh packages/deb \
	autogen.sh $(SCRIPTS) src/solaris/libstdc++.la

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

EXTRA_DIST += libtool

CLEANFILES = $(protoc_outputs) protoc_gen

