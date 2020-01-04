//
// Created by diego on 28/12/19.
//

#include <numeric>
#include <tuple>
#include "rle.h"

rle::rle(vector<short> vect){
    rle::vect = vect;
}

rle::rle(vector<tuple<short, short>> vect){
    rle::vect_rle = vect;
}

vector<short> rle::rm_fin_zeros(){

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

    vector<short> res(vect.begin(),vect.begin()+idx);
    return res;

}

vector<short> rle::reverse_rle_zeros(int num_zeros){

    vector<short> vect_null(num_zeros, 0);
    vect.insert( vect.end(), vect_null.begin(), vect_null.end());
    return vect;

}

vector<short> rle::it_zero_rm(vector<short> vec) {
    int idx = -1, count = 0;

    for(int x : vec){
        if(idx==-1 && x==0) idx=count;
        else if(x!=0) idx=-1;
        count++;
    }
    vector<short> res(vec.begin(), vec.begin()+idx);
    return res;
}


vector<short> rle::rec_zero_rm(vector<short> tmp_vec){
    int middle_pnt = int(tmp_vec.size()/2);

    if(tmp_vec.at(middle_pnt)==0){
        vector<short> tmp_post(tmp_vec.begin()+middle_pnt, tmp_vec.end());
        vector<short> tmp_pre(tmp_vec.begin(),tmp_vec.begin()+middle_pnt);

        if(accumulate(tmp_post.begin(), tmp_post.end(), 0) == 0)
            return rle::rec_zero_rm(tmp_pre);

        return rle::it_zero_rm(tmp_pre);
    }

    return rle::it_zero_rm(tmp_vec);
}

vector<tuple<short, short>> rle::get_rle(){
    vector<tuple<short, short>> tmp(vect.size()+1,std::make_tuple(0,0));
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

vector<tuple<short, short>> rle::get_variant_rle(){
    vector<tuple<short, short>> tmp(vect.size()+1,std::make_tuple(0,0));
    int count_zeros = 0;
    int idx = 0;
    auto it = vect.begin();
    while(it != vect.end()){
        if(*it!=0){
            tmp[idx++] = make_tuple(count_zeros,*it);
            count_zeros = 0;
        }else{
            count_zeros++;
        }
        ++it;
    }
    tmp[idx++] = make_tuple(0,0);
    tmp.erase (tmp.begin()+idx,tmp.end());
    return tmp;
}

int rle::get_vector_size() {
    int count=0;
    for(tuple<short,short>t:vect_rle)
        count+=std::get<1>(t);
    return count;
}

int rle::get_variant_vector_size() {
    int count=0;
    for(tuple<short,short>t:vect_rle)
        count+=std::get<0>(t) + 1;
    return count -1;
}

vector<short> rle::load_rle(){
    vector<short> res(get_vector_size(),0);
    int idx=0;
    for(tuple<short,short>t:vect_rle){
        for(int i=0; i< std::get<1>(t); i++)
            res[idx++] = std::get<0>(t);
    }
    return res;
}

vector<short> rle::load_variant_rle(int n){
    assert(n>=get_variant_vector_size());
    vector<short> res(n,0);
    int idx=0;
    for(tuple<short,short>t:vect_rle){
        for(int i=0; i< std::get<0>(t); i++) res[idx++] = 0;
        res[idx++] = std::get<1>(t);
    }
    return res;
}

int rle::get_num_zeros(){
    if(rle::num_zeros==-1) rm_fin_zeros();
    return rle::num_zeros;
}

vector<short> rle::get_vector(){return vect;}

vector<tuple<short, short>> rle::get_vector_rle(){return vect_rle;}


