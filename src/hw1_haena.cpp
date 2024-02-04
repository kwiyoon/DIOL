#include "hw1_haena.h"

using namespace std;




bool LSM::isFull(){

    if(Mem.size()>MAX){
        return 1;
    }else{
        return 0;
    }

}

bool LSM::compaction(){
    Disk=Mem;

    return 1;
}

void LSM::insertData(unsigned int key, int value){

/*    if(isFull()){ //가득찼다면
        if(compaction()){ //compaction 성공

        }else{  //compaction 실패
            cout<<"compaction fail\n";
            return 0; //insert 실패
        }
    }*/
//        Mem[key]=value;

    cout << "hi "<<key << ","<< value <<" bye\n";
    Mem.emplace(key, value);
    cout << "hi "<<Mem.begin()->first << ","<< Mem.begin()->second <<" bye\n";

//    return 0;

}

int LSM::readData(int64_t key){

    return Mem[key];

}

