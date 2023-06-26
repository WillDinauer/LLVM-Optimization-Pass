#
# Makefile to build Project 2 - Constant Folding and Constant Propagation Pass
# This Makefile was created using the provided sample which is described as follows:
#
# 'Sample Makefile to generate LLVM passes
#
# make all builds the sample template as a shared library
# make test runs the library against an example IR
# make clean deletes the built files
#
# 05 May 2022  jpb  Creation.'
# 
# William Dinauer, Dartmouth CS57 Spring 2022
#

CC=clang-10
CFLAGS=`llvm-config-10 --cflags` -fPIC
CXX=clang++-10
CXXFLAGS=`llvm-config-10 --cxxflags` -fPIC
LDFLAGS=`llvm-config-10 --ldflags`
OPT=opt-10

TEMPLATENAME=FoldPropPass

all: $(TEMPLATENAME).so

$(TEMPLATENAME).so:	$(TEMPLATENAME).o
	$(CXX) --shared -o $(TEMPLATENAME).so ${LDFLAGS} $^

$(TEMPLATENAME).o: $(TEMPLATENAME).cpp
	$(CXX) $(CXXFLAGS) -c $^ -o $@

test:	$(TEMPLATENAME).so
	$(OPT) -S -load ./$(TEMPLATENAME).so -foldproppass < testFiles/mytest.ll > /dev/null

clean:
	$(RM) $(TEMPLATENAME).o $(TEMPLATENAME).so
