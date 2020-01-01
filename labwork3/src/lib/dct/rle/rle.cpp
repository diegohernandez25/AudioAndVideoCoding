//
// Created by diego on 28/12/19.
//

#include <numeric>
#include <tuple>
#include "rle.h"

rle::rle(vector<int> vect){
    rle::vect = vect;
}

rle::rle(vector<tuple<int, int>> vect){
    rle::vect_rle = vect;
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

vector<int> rle::reverse_rle_zeros(int num_zeros){

    vector<int> vect_null(num_zeros, 0);
    vect.insert( vect.end(), vect_null.begin(), vect_null.end());
    return vect;

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

vector<tuple<int, int>> rle::get_rle(){
    vector<tuple<int, int>> tmp(vect.size()+1,std::make_tuple(0,0));
    int current_val = vect[0];
    int count_current_val = 1;
    int idx = 0;
    auto it = vect.begin();
    advance(it,1);
    while(it != vect.end()){
        if(*it!=current_val){
            tmp[idx++] = make_tuple(current_val,count_current_val);
            current_val = *it;
            count_current_val = 1;
        }else{
            count_current_val++;
        }
        ++it;
    }

    tmp[idx++] = make_tuple(current_val, count_current_val);
    tmp[idx++] = make_tuple(0,0);
    tmp.erase (tmp.begin()+idx,tmp.end());
    return tmp;
}

int rle::get_vector_size() {
    int count=0;
    for(tuple<int,int>t:vect_rle)
        count+=std::get<1>(t);
    return count;
}

vector<int> rle::load_rle(){
    vector<int> res(get_vector_size(),0);
    int idx=0;
    for(tuple<int,int>t:vect_rle){
        for(int i=0; i< std::get<1>(t); i++)
            res[idx++] = std::get<0>(t);
    }
    return res;
}

int rle::get_num_zeros(){
    if(rle::num_zeros==-1) rm_fin_zeros();
    return rle::num_zeros;
}

vector<int> rle::get_vector(){return vect;}

vector<tuple<int, int>> rle::get_vector_rle(){return vect_rle;}


