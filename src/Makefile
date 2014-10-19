TARGETS=cidiff
CXXFLAGS+=-Wall -Wextra -I../../freeimage/Source -I../../freeimage/Wrapper/FreeImagePlus
SRCS=cidiff.cpp
OBJS=$(SRCS:.cpp=*.o)
HEADERS=ImgDiffBuffer.hpp ImgMergeBuffer.hpp image.hpp
LIBS=-L../../freeimage/ -lfreeimage -L../../freeimage/ -lfreeimageplus

all: $(TARGETS)

clean:
	@rm -f $(TARGETS) $(OBJS)

%.o : %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

cidiff: cidiff.o
	$(CXX) $< $(LIBS) -o $@


