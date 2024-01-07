SRCS = $(shell find . -name "*.cpp") 

OBJS = $(addsuffix .o, $(basename $(SRCS)))

EXEC = Local-ILP

LIBS = -lpthread -pthread -lz -lm -lboost_thread -lboost_date_time -lboost_system -static

CXXFLAGS = -I. -std=c++11 -O0

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)