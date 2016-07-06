ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	CMakeLists.txt	\
	Doxyfile.in.cmake

html-local:
if HAVE_DOXYGEN
	$(DOXYGEN)
endif

pdf-local:
if HAVE_DOXYGEN
	$(DOXYGEN)
endif
