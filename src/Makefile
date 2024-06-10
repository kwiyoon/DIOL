# 컴파일러 및 플래그 설정
CXX = g++
CXXFLAGS = -std=c++17 -I./src
LDFLAGS = -lstdc++fs -lstdc++ -pthread

# 타겟 정의
all: makeDataset testDIOL makeWorkload DtestDIOL

# makeDataset 타겟
makeDataset: makeDataset.o DataFactory.o DBManager.o IMemtable.o DelayMemtable.o NormalMemtable.o MockDisk.o SSTable.o
	$(CXX) -o makeDataset makeDataset.o DataFactory.o DBManager.o IMemtable.o DelayMemtable.o NormalMemtable.o MockDisk.o SSTable.o $(LDFLAGS)

# testDIOL 타겟
testDIOL: DBManager.o IMemtable.o DelayMemtable.o NormalMemtable.o MockDisk.o SSTable.o DataFactory.o testSSTable.o CompactionTest.o Workload.o testDIOL.o ActiveMemtableController.o ImmutableMemtableController.o CompactionController.o CompactProcessor.o DelayDetector.o FlushController.o
	$(CXX) -o testDIOL DBManager.o IMemtable.o DelayMemtable.o NormalMemtable.o MockDisk.o SSTable.o DataFactory.o testSSTable.o CompactionTest.o Workload.o testDIOL.o ActiveMemtableController.o ImmutableMemtableController.o CompactionController.o CompactProcessor.o DelayDetector.o FlushController.o $(LDFLAGS)

# makeWorkload 타겟
makeWorkload: makeWorkload.o Workload.o DataFactory.o DBManager.o IMemtable.o DelayMemtable.o NormalMemtable.o MockDisk.o SSTable.o
	$(CXX) -o makeWorkload makeWorkload.o Workload.o DataFactory.o DBManager.o IMemtable.o DelayMemtable.o NormalMemtable.o MockDisk.o SSTable.o $(LDFLAGS)

# 개별 소스 파일을 오브젝트 파일로 컴파일하는 규칙
makeDataset.o: makeDataset.cpp
	$(CXX) $(CXXFLAGS) -c makeDataset.cpp -o makeDataset.o

makeWorkload.o: makeWorkload.cpp
	$(CXX) $(CXXFLAGS) -c makeWorkload.cpp -o makeWorkload.o

DataFactory.o: test/DataFactory.cpp
	$(CXX) $(CXXFLAGS) -c test/DataFactory.cpp -o DataFactory.o

DBManager.o: diol/DBManager.cpp
	$(CXX) $(CXXFLAGS) -c diol/DBManager.cpp -o DBManager.o

IMemtable.o: diol/memtable/IMemtable.cpp
	$(CXX) $(CXXFLAGS) -c diol/memtable/IMemtable.cpp -o IMemtable.o

DelayMemtable.o: diol/memtable/DelayMemtable.cpp
	$(CXX) $(CXXFLAGS) -c diol/memtable/DelayMemtable.cpp -o DelayMemtable.o

NormalMemtable.o: diol/memtable/NormalMemtable.cpp
	$(CXX) $(CXXFLAGS) -c diol/memtable/NormalMemtable.cpp -o NormalMemtable.o

MockDisk.o: diol/MockDisk.cpp
	$(CXX) $(CXXFLAGS) -c diol/MockDisk.cpp -o MockDisk.o

SSTable.o: diol/SSTable.cpp
	$(CXX) $(CXXFLAGS) -c diol/SSTable.cpp -o SSTable.o

testSSTable.o: test/testSSTable.cpp
	$(CXX) $(CXXFLAGS) -c test/testSSTable.cpp -o testSSTable.o

CompactionTest.o: test/CompactionTest.cpp
	$(CXX) $(CXXFLAGS) -c test/CompactionTest.cpp -o CompactionTest.o

Workload.o: test/workload/Workload.cpp
	$(CXX) $(CXXFLAGS) -c test/workload/Workload.cpp -o Workload.o

testDIOL.o: testDIOL.cpp
	$(CXX) $(CXXFLAGS) -c testDIOL.cpp -o testDIOL.o

ActiveMemtableController.o: diol/memtableController/ActiveMemtableController.cpp
	$(CXX) $(CXXFLAGS) -c diol/memtableController/ActiveMemtableController.cpp -o ActiveMemtableController.o

ImmutableMemtableController.o: diol/memtableController/ImmutableMemtableController.cpp
	$(CXX) $(CXXFLAGS) -c diol/memtableController/ImmutableMemtableController.cpp -o ImmutableMemtableController.o

CompactionController.o: diol/CompactionController.cpp
	$(CXX) $(CXXFLAGS) -c diol/CompactionController.cpp -o CompactionController.o

CompactProcessor.o: diol/CompactProcessor.cpp
	$(CXX) $(CXXFLAGS) -c diol/CompactProcessor.cpp -o CompactProcessor.o

DelayDetector.o: diol/DelayDetector.cpp
	$(CXX) $(CXXFLAGS) -c diol/DelayDetector.cpp -o DelayDetector.o

FlushController.o: diol/FlushController.cpp
	$(CXX) $(CXXFLAGS) -c diol/FlushController.cpp -o FlushController.o

# 클린업 타겟
clean:
	rm -f makeDataset testDIOL makeWorkload
	rm -f *.o

.PHONY: all clean
