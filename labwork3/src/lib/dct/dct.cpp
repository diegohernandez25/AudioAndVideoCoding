//
// Created by diego on 29/12/19.
//

#include "dct.h"

dct::dct(int height, int width, cv::Size block_size, int quant_lvl, bool chroma){
    assert(quant_lvl>0);

    this->block_size=block_size;
    if(!chroma)
        this->quant_mat=cv::Mat(BLOCK_SIZE_DEFAULT, BLOCK_SIZE_DEFAULT, CV_16S, this->quant_array)/quant_lvl;
    else
        this->quant_mat=cv::Mat(BLOCK_SIZE_DEFAULT, BLOCK_SIZE_DEFAULT, CV_16S, this->quant_array_chroma)/quant_lvl;

    this->subtraction_mat=cv::Mat(block_size.height,block_size.width,CV_32F,128);

    if(block_size.height!=block_size.width || block_size.width!=BLOCK_SIZE_DEFAULT || block_size.height!=BLOCK_SIZE_DEFAULT)
        change_quant_mats();

    this->height_blk = ceil((float) height/block_size.height);
    this->width_blk = ceil((float) width/block_size.width);
    this->height_padded = height_blk * block_size.height;
    this->width_padded = width_blk * block_size.width;
    this->total_blk = height_blk * width_blk;
    this->rstr_scnr_blk_ptr = 0;
    this->vect.reserve(height_blk * width_blk);
    this->rcvrd_image = cv::Mat(height_padded, width_padded, CV_16S, cv::Scalar(0));;
    this->padded_image.create(height_padded, width_padded, 0);
}

dct::dct(cv::Mat image, cv::Size block_size, int quant_lvl,  bool chroma) : dct(image.rows, image.cols, block_size, quant_lvl, chroma) {
    this->image = image;
    image.copyTo(padded_image(cv::Rect(0,0,image.cols, image.rows)));
    this->padded_image.convertTo(padded_image, CV_8UC1);
}


vector<tuple<short,short>> dct::dct_quant_rle_blck(cv::Mat blck, bool mask) {
    cv::Mat  block, dct_block, quant_block;

    blck.convertTo(block,CV_32F);
    block=block-this->subtraction_mat;

    cv::dct(block, dct_block);
    dct_block.convertTo(dct_block, CV_16S);

    if(mask) cv::multiply(dct_block, quant_mat_mask, quant_block);
    else divide(dct_block,quant_mat, quant_block);


    zigzag z = zigzag(quant_block);
    vector<short> tmp = z.load_zigzag();
    rle r = rle(tmp);

    return r.get_variant_rle();
}

cv::Mat dct::reverse_dct_quant_rle_blck(vector<tuple<short , short>> rle_vct, bool mask){
    cv::Mat res, dct_block, quant_block, zz_block;

    rle r_tmp=rle(rle_vct);
    vector<short>  zigzag_vct=r_tmp.load_variant_rle(block_size.height*block_size.width);
    zigzag z=zigzag(zigzag_vct,block_size.height,block_size.width); 
    zz_block=z.inverse_zigzag();

    if(mask) cv::multiply(zz_block,quant_mat_mask,quant_block);
    else cv::multiply(zz_block,quant_mat,quant_block);

    quant_block.convertTo(quant_block, CV_32F);
    cv::dct(quant_block, dct_block, cv::DCT_INVERSE);
    dct_block=dct_block+this->subtraction_mat;
    dct_block.convertTo(res, CV_16S);
    return res;
}

vector<vector<tuple<short,short>>> dct::dct_quant_rle_frame(bool mask){

    cv::Mat tmp_block;
    vector<tuple<short,short>> tmp_vct_rle;

    for(int i = 0; i < height_blk; i++){
        for(int j = 0; j< width_blk; j++){
            tmp_block = padded_image(cv::Rect(j*block_size.width,i*block_size.height, block_size.width, block_size.height));
            tmp_vct_rle = dct_quant_rle_blck(tmp_block, mask);
            this->vect.push_back(tmp_vct_rle);
        }
    }
    return this->vect;
}

cv::Mat dct::reverse_dct_quant_rle_frame(vector<vector<tuple<short,short>>> rle_vctrs, bool mask){
    int row_blk=0, col_blk=0;
    cv::Mat tmp_block;

    cv::Mat final(height_padded, width_padded, CV_16S, cv::Scalar(0));

    for(vector<tuple<short,short>> x: rle_vctrs){
        tmp_block = dct::reverse_dct_quant_rle_blck(x, mask);
        tmp_block.copyTo(final(cv::Rect(col_blk*block_size.width, row_blk*block_size.height,block_size.width, block_size.height)));
        col_blk = (col_blk +1) % dct::width_blk;
        if(col_blk==0) row_blk++;
    }
    final.convertTo(final, CV_8UC1);
    return final;
}

bool dct::upload_coded_blck(vector<tuple<short, short>> rle_vct, bool mask){
    int row_blk, col_blk;
    cv::Mat tmp_block;

    tmp_block = reverse_dct_quant_rle_blck(std::move(rle_vct), mask);
    row_blk = (int) rstr_scnr_blk_ptr/width_blk;
    col_blk = (int) rstr_scnr_blk_ptr%width_blk;
    tmp_block.copyTo(rcvrd_image(cv::Rect(col_blk*block_size.width, row_blk*block_size.height,block_size.width, block_size.height)));
    rstr_scnr_blk_ptr = (rstr_scnr_blk_ptr + 1)%total_blk;
    return  rstr_scnr_blk_ptr==0;
}

void dct::change_quant_mats() {

    int limit = (block_size.height>block_size.width)?block_size.height:block_size.width;
    int size = (int)pow((float)limit,2);
    vector<short> quant_vect;
    quant_vect.reserve(size);

    for (int j = limit; j >=1; j--){
        vector<short> v_ones(j,1);
        vector<short> v_zeros(limit-j,0);
        v_ones.insert(v_ones.end(),v_zeros.begin(),v_zeros.end());
        quant_vect.insert(quant_vect.end(),v_ones.begin(), v_ones.end());
    }

    short r[size];
    std::copy(quant_vect.begin(),quant_vect.end(),r);
    this->quant_mat_mask = cv::Mat(limit, limit, CV_16S, r);
    this->quant_mat_mask = this->quant_mat_mask(cv::Rect(0,0,block_size.width,block_size.height));

    if(block_size.width<=BLOCK_SIZE_DEFAULT && block_size.height<=BLOCK_SIZE_DEFAULT)
        this->quant_mat = quant_mat(cv::Rect(0,0,block_size.width, block_size.height));
    else
        cv::resize(this->quant_mat,this->quant_mat,cv::Size(block_size.width,block_size.height));

}

int dct::get_height_blk() {return height_blk;}
int dct::get_width_blk(){return width_blk;}
int dct::get_total_blk(){return total_blk;}
int dct::get_height_padded(){return height_padded;}
int dct::get_width_padded(){return width_padded;}
int dct::get_rstr_scnr_blk_ptr(){return rstr_scnr_blk_ptr;}
cv::Mat dct::get_image(){return image;}
cv::Mat dct::get_padded_image(){return padded_image;}
cv::Mat dct::get_rcvrd_image(){return rcvrd_image;}
vector<vector<tuple<short,short>>> dct::get_vect(){ return dct::vect;}
cv::Size dct::get_block_size(){ return block_size;}
