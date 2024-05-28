CXX = g++
CXXFLAGS = -std=c++17 -lstdc++fs -I./src

# 타겟 정의
all: testDIOL

# 소스 파일 목록
SRCS = \
    diol/memtable/DelayMemtable.cpp \
    diol/memtable/IMemtable.cpp \
    diol/memtable/NormalMemtable.cpp \
    diol/memtableController/ActiveMemtableController.cpp \
    diol/memtableController/ImmutableMemtableController.cpp \
    diol/CompactionController.cpp \
    diol/CompactProcessor.cpp \
    diol/DBManager.cpp \
    diol/DelayDetector.cpp \
    diol/FlushController.cpp \
    diol/MockDisk.cpp \
    diol/SSTable.cpp \
    testDIOL.cpp

# 오브젝트 파일 목록
OBJS = $(SRCS:.cpp=.o)

# 실행 파일 이름
EXEC = testDOIL

# 기본 타겟
all: $(EXEC)

# 실행 파일 빌드 규칙
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lstdc++fs

# 개별 오브젝트 파일 빌드 규칙
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 정리 규칙
clean:
	rm -f $(OBJS) $(EXEC)

# 실행 규칙
run: $(EXEC)
	./$(EXEC)

# 디버그 빌드 규칙
debug: CXXFLAGS += -g
debug: clean all