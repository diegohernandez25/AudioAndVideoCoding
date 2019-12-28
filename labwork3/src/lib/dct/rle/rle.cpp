//
// Created by diego on 28/12/19.
//

#include <numeric>
#include "rle.h"

rle::rle(vector<int> vect){
    rle::vect = vect;
}

vector<int> rle::rm_fin_zeros(){

    int idx=-1,count = 0;

    for(int x : vect){
        if(idx==-1 && x==0) idx=count;
        else if(x!=0) idx=-1;
        count++;
    }

    if(idx==-1) {
        rle::num_zeros = 0;
        idx = 0;
    }
    else
        rle::num_zeros = vect.size() - idx;

    vector<int> res(vect.begin(),vect.begin()+idx);
    return res;

}


vector<int> rle::it_zero_rm(vector<int> vec) {
    int idx = -1, count = 0;

    for(int x : vec){
        if(idx==-1 && x==0) idx=count;
        else if(x!=0) idx=-1;
        count++;
    }
    vector<int> res(vec.begin(), vec.begin()+idx);
    return res;

}


vector<int> rle::rec_zero_rm(vector<int> tmp_vec){
    int middle_pnt = int(tmp_vec.size()/2);

    if(tmp_vec.at(middle_pnt)==0){
        vector<int> tmp_post(tmp_vec.begin()+middle_pnt, tmp_vec.end());
        vector<int> tmp_pre(tmp_vec.begin(),tmp_vec.begin()+middle_pnt);

        if(accumulate(tmp_post.begin(), tmp_post.end(), 0) == 0)
            return rle::rec_zero_rm(tmp_pre);

        return rle::it_zero_rm(tmp_pre);
    }

    return rle::it_zero_rm(tmp_vec);
}


int rle::get_num_zeros(){
    if(rle::num_zeros==-1) rm_fin_zeros();
    return rle::num_zeros;
}

vector<int> rle::get_vector(){
    return vect;
}

