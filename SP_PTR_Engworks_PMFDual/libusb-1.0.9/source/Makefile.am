AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip
ACLOCAL_AMFLAGS = -I m4
DISTCLEANFILES = libusb-1.0.pc
MAINTAINERCLEANFILES = ChangeLog
EXTRA_DIST = TODO PORTING msvc
SUBDIRS = libusb doc

if BUILD_EXAMPLES
SUBDIRS += examples
endif

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libusb-1.0.pc

.PHONY: ChangeLog dist-up
ChangeLog:
	git --git-dir $(top_srcdir)/.git log > ChangeLog || touch ChangeLog

dist-hook: ChangeLog

reldir = .release/$(distdir)
dist-up: dist
	rm -rf $(reldir)
	mkdir -p $(reldir)
	cp $(distdir).tar.bz2 $(reldir)
	rsync -rv $(reldir) frs.sourceforge.net:/home/frs/project/l/li/libusb/libusb-1.0/
	rm -rf $(reldir)

