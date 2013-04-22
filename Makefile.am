AM_CXXFLAGS = -Wall -Werror -g -O0 -I$(top_srcdir)/src -I/software/Linux64/include -std=c++0x

# Yaggo variable
noinst_HEADERS = $(YAGGO_SOURCES)
BUILT_SOURCES = $(YAGGO_SOURCES)
DISTCLEANFILES = $(BUILT_SOURCES)

# Yaggo automatic rules with silencing
V_YAGGO = $(V_YAGGO_$(V))
V_YAGGO_ = $(V_YAGGO_$(AM_DEFAULT_VERBOSITY))
V_YAGGO_0 = @echo "  YAGGO " $@;
.yaggo.hpp:
	$(V_YAGGO)$(YAGGO) -l $(top_srcdir)/header-license -o $@ $<
YAGGO_SOURCES = # Append all file to be built by yaggo

##############################
# Programs
##############################
bin_PROGRAMS = horient

horient_SOURCES = src/horient.cc src/readdata.cpp src/findbest.cpp	\
                  src/findloss.cpp src/pickflip.cpp
noinst_HEADERS += src/horient.hpp src/edge_list.hpp src/node.hpp
YAGGO_SOURCES += src/horient_cmdline.hpp

##############################
# Unit tests
##############################
TESTS = all_tests
check_PROGRAMS = all_tests
all_tests_CXXFLAGS = $(AM_CXXFLAGS) -I$(top_srcdir)/unittests
all_tests_LDFLAGS = -lpthread
all_tests_SOURCES = unittests/gtest/gtest-all.cc unittests/gtest/gtest_main.cc unittests/gtest/gtest.h

all_tests_SOURCES += unittests/test_edge.cc unittests/test_node.cc	\
                      unittests/test_read_data.cc src/readdata.cpp	\
                      src/findloss.cpp unittests/test_pickflip.cc	\
                      src/pickflip.cpp src/findbest.cpp			\
                      unittests/test_merge.cc				\
                      unittests/test_orientation_tree.cc

######################
# Actual graph tests #
######################
TESTS += test_graphs.sh
test_graphs.sh: test_graphs.sh.in Makefile
	sed -e 's|[@]top_srcdir[@]|$(top_srcdir)|g' < $< > $@
	chmod +x $@
