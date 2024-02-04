#include <gtest/gtest.h>
#include <map>
#include <vector>
#include "hw1_nagyeongLSM.h"
using namespace std;



int insertData(unsigned int key, int value){
    //데이터가 들어왔을때 isFull인지 체크하고 full이 아니면 insert, full이면 compaction 결정하는 로직
    if(isFull()){
//        for(auto iter = lsmTree.begin(); iter != lsmTree.end(); iter++){
//            cout<< "key : "<<iter->first<< " val : "<< iter->second<<endl;
//        }
//        cout<<"data 가득 찼음 compaction시작\n";
        compaction();
//        cout<<"compaction complete!\n";
    }
//    lsmTree.insert(  Int_Pair(key, value) );
    lsmTree[key]=value;

//    for(auto iter = lsmTree.begin(); iter != lsmTree.end(); iter++){
//            cout<< "key : "<<iter->first<< " val : "<< iter->second<<endl;
//        }
//cout<<"insert end \n";
    return 0;
}
bool isFull(){
    //트리 크기(10)만큼 데이터가 다 찼는지
    if(lsmTree.size()>=10)
        return true;

    else
        return false;
}
int readData(int key){
    auto target = lsmTree.find(key);
    if (target != lsmTree.end()) {
//        cout<<"찾을 key : "<<key<<" 찾은 value : "<<it->second<<endl;
        return target->second;
    } else {
        cout<<"해당 key("<< key << ") 찾을 수 없음\n";

        return -1;
    }
}
bool compaction(){
    //현재 메모리인 lsm-tree는 비우고 disk에 10개 데이터 요소 push
    map<int, int> disk;
    for (auto& entry : lsmTree) {
        disk[entry.first] = entry.second;
    }
    lsmTree.clear();

    return (lsmTree.size() == 0) ? true : false;

}