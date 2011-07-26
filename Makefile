#############################################################################
# Makefile for building: sunshine
# Generated by qmake (2.01a) (Qt 4.7.2) on: Tue Jul 26 04:58:37 2011
# Project:  sunshine.pro
# Template: subdirs
# Command: /usr/bin/qmake -o Makefile sunshine.pro
#############################################################################

first: make_default
MAKEFILE      = Makefile
QMAKE         = /usr/bin/qmake
DEL_FILE      = rm -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
COPY          = cp -f
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
INSTALL_FILE  = install -m 644 -p
INSTALL_PROGRAM = install -m 755 -p
INSTALL_DIR   = $(COPY_DIR)
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p
SUBTARGETS    =  \
		sub-PythonQt2-0-1 \
		sub-src

PythonQt2.0.1//$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) PythonQt2.0.1/ || $(MKDIR) PythonQt2.0.1/ 
	cd PythonQt2.0.1/ && $(QMAKE) /home/stratton/sunshine/PythonQt2.0.1/PythonQt2.0.1.pro -o $(MAKEFILE)
sub-PythonQt2-0-1-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) PythonQt2.0.1/ || $(MKDIR) PythonQt2.0.1/ 
	cd PythonQt2.0.1/ && $(QMAKE) /home/stratton/sunshine/PythonQt2.0.1/PythonQt2.0.1.pro -o $(MAKEFILE)
sub-PythonQt2-0-1: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE)
sub-PythonQt2-0-1-make_default-ordered: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) 
sub-PythonQt2-0-1-make_default: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) 
sub-PythonQt2-0-1-make_first-ordered: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) first
sub-PythonQt2-0-1-make_first: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) first
sub-PythonQt2-0-1-all-ordered: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) all
sub-PythonQt2-0-1-all: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) all
sub-PythonQt2-0-1-clean-ordered: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) clean
sub-PythonQt2-0-1-clean: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) clean
sub-PythonQt2-0-1-distclean-ordered: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) distclean
sub-PythonQt2-0-1-distclean: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) distclean
sub-PythonQt2-0-1-install_subtargets-ordered: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) install
sub-PythonQt2-0-1-install_subtargets: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) install
sub-PythonQt2-0-1-uninstall_subtargets-ordered: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) uninstall
sub-PythonQt2-0-1-uninstall_subtargets: PythonQt2.0.1//$(MAKEFILE) FORCE
	cd PythonQt2.0.1/ && $(MAKE) -f $(MAKEFILE) uninstall
src//$(MAKEFILE): 
	@$(CHK_DIR_EXISTS) src/ || $(MKDIR) src/ 
	cd src/ && $(QMAKE) /home/stratton/sunshine/src/src.pro -o $(MAKEFILE)
sub-src-qmake_all:  FORCE
	@$(CHK_DIR_EXISTS) src/ || $(MKDIR) src/ 
	cd src/ && $(QMAKE) /home/stratton/sunshine/src/src.pro -o $(MAKEFILE)
sub-src: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE)
sub-src-make_default-ordered: src//$(MAKEFILE) sub-PythonQt2-0-1-make_default-ordered  FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) 
sub-src-make_default: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) 
sub-src-make_first-ordered: src//$(MAKEFILE) sub-PythonQt2-0-1-make_first-ordered  FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) first
sub-src-make_first: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) first
sub-src-all-ordered: src//$(MAKEFILE) sub-PythonQt2-0-1-all-ordered  FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) all
sub-src-all: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) all
sub-src-clean-ordered: src//$(MAKEFILE) sub-PythonQt2-0-1-clean-ordered  FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) clean
sub-src-clean: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) clean
sub-src-distclean-ordered: src//$(MAKEFILE) sub-PythonQt2-0-1-distclean-ordered  FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-distclean: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) distclean
sub-src-install_subtargets-ordered: src//$(MAKEFILE) sub-PythonQt2-0-1-install_subtargets-ordered  FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) install
sub-src-install_subtargets: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) install
sub-src-uninstall_subtargets-ordered: src//$(MAKEFILE) sub-PythonQt2-0-1-uninstall_subtargets-ordered  FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) uninstall
sub-src-uninstall_subtargets: src//$(MAKEFILE) FORCE
	cd src/ && $(MAKE) -f $(MAKEFILE) uninstall

Makefile: sunshine.pro  /usr/share/qt4/mkspecs/linux-g++/qmake.conf /usr/share/qt4/mkspecs/common/g++.conf \
		/usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/release.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf
	$(QMAKE) -o Makefile sunshine.pro
/usr/share/qt4/mkspecs/common/g++.conf:
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/release.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
qmake: qmake_all FORCE
	@$(QMAKE) -o Makefile sunshine.pro

qmake_all: sub-PythonQt2-0-1-qmake_all sub-src-qmake_all FORCE

make_default: sub-PythonQt2-0-1-make_default-ordered sub-src-make_default-ordered FORCE
make_first: sub-PythonQt2-0-1-make_first-ordered sub-src-make_first-ordered FORCE
all: sub-PythonQt2-0-1-all-ordered sub-src-all-ordered FORCE
clean: sub-PythonQt2-0-1-clean-ordered sub-src-clean-ordered FORCE
distclean: sub-PythonQt2-0-1-distclean-ordered sub-src-distclean-ordered FORCE
	-$(DEL_FILE) Makefile
install_subtargets: sub-PythonQt2-0-1-install_subtargets-ordered sub-src-install_subtargets-ordered FORCE
uninstall_subtargets: sub-PythonQt2-0-1-uninstall_subtargets-ordered sub-src-uninstall_subtargets-ordered FORCE

sub-PythonQt2-0-1-check_ordered: PythonQt2.0.1/$(MAKEFILE)
	cd PythonQt2.0.1/ && $(MAKE) check
sub-src-check_ordered: src/$(MAKEFILE) sub-PythonQt2-0-1-check_ordered 
	cd src/ && $(MAKE) check
check: sub-PythonQt2-0-1-check_ordered sub-src-check_ordered

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all
install: install_subtargets  FORCE

uninstall:  uninstall_subtargets FORCE

FORCE:

