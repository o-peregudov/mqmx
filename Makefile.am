ACLOCAL_AMFLAGS = -I m4
nodist_doc_DATA = README

EXTRA_DIST =	CMakeLists.txt		\
		mqmx/CMakeLists.txt	\
		test/CMakeLists.txt	\
		Common/CMakeLists.txt	\
		Common/OAMThreading.hpp

SUBDIRS = mqmx test

@CODE_COVERAGE_RULES@

clean-local:
	-find . -name "*.gcda" -o -name "*.gcov" -o -name "*.gcno" -delete
