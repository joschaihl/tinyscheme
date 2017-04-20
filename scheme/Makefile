CXXFLAGS =	-std=c++11 -pedantic -g -Wall -fmessage-length=0

OBJS =		scheme.o Interpreter.o

LIBS =

TARGET =	scheme

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
