//
// Created by diego on 29/12/19.
//

#include "dct.h"

dct::dct(int height, int width){
    height_blk = ceil((float) height/BLOCK_SIZE);
    width_blk  = ceil((float) width/BLOCK_SIZE);
    height_padded = height_blk * BLOCK_SIZE;
    width_padded  = width_blk * BLOCK_SIZE;
    total_blk  = height_blk * width_blk;
    rstr_scnr_blk_ptr = 0;

    vector<vector<tuple<int,int>>> tmp;
    tmp.reserve(height_blk * width_blk);
    dct::vect = tmp;

    cv::Mat tmp_mat(height_padded, width_padded, CV_8UC1, cv::Scalar(0));
    rcvrd_image = tmp_mat;

    padded_image.create(height_padded, width_padded, 0);

}

dct::dct(cv::Mat image) : dct(image.rows, image.cols) {
    this->image = image;
    image.copyTo(padded_image(cv::Rect(0,0,image.cols, image.rows)));
    padded_image.convertTo(padded_image, CV_32F);
}


vector<tuple<int,int>> dct::dct_quant_rle_blck(cv::Mat blck) {
    cv::Mat  dct_block, quant_block;

    cv::dct(blck, dct_block);
    dct_block.convertTo(dct_block,CV_32S);
    divide(dct_block,quant_mat, quant_block);
    zigzag z = zigzag(quant_block);
    vector<int> tmp = z.load_zigzag();
    rle r = rle(tmp);

    return r.get_rle();
}

cv::Mat dct::reverse_dct_quant_rle_blck(vector<tuple<int, int>> rle_vct){
    cv::Mat res, dct_block, quant_block;
    vector<int> tmp_vct;

    rle r_tmp = rle(rle_vct);
    vector<int>  zigzag_vct = r_tmp.load_rle();
    zigzag z_tmp = zigzag(zigzag_vct);
    quant_block  = z_tmp.inverse_zigzag();
    cv::multiply(quant_block,quant_mat,dct_block);
    dct_block.convertTo(quant_block, CV_32F);
    cv::dct(quant_block, quant_block, cv::DCT_INVERSE);
    quant_block.convertTo(res, CV_8UC1);

    return res;
}

void dct::dct_quant_rle_frame(){

    cv::Mat tmp_block;
    vector<int> tmp_vct;
    vector<tuple<int,int>> tmp_vct_rle;

    for(int i = 0; i < height_blk; i++){
        for(int j = 0; j< width_blk; j++){
            tmp_block = padded_image(cv::Rect(j*BLOCK_SIZE,i*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
            tmp_vct_rle = dct_quant_rle_blck(tmp_block);
            dct::vect.push_back(tmp_vct_rle);
        }
    }
}

cv::Mat dct::reverse_dct_quant_rle_frame(vector<vector<tuple<int,int>>> rle_vctrs){
    int row_blk=0, col_blk=0;
    cv::Mat tmp_block;
    vector<int> tmp_vct;

    cv::Mat final(height_padded, width_padded, CV_8UC1, cv::Scalar(0));

    for(vector<tuple<int,int>> x: rle_vctrs){
        tmp_block = dct::reverse_dct_quant_rle_blck(x);
        tmp_block.copyTo(final(cv::Rect(col_blk*BLOCK_SIZE, row_blk*BLOCK_SIZE,BLOCK_SIZE, BLOCK_SIZE)));
        col_blk = (col_blk +1) % dct::width_blk;
        if(col_blk==0) row_blk++;
    }

    return final;
}

bool dct::upload_coded_blck(vector<tuple<int, int>> rle_vct){
    int row_blk, col_blk;
    cv::Mat tmp_block;
    vector<int> tmp_vct;

    tmp_block = reverse_dct_quant_rle_blck(std::move(rle_vct));
    row_blk = (int) rstr_scnr_blk_ptr/width_blk;
    col_blk = (int) rstr_scnr_blk_ptr%width_blk;
    tmp_block.copyTo(rcvrd_image(cv::Rect(col_blk*BLOCK_SIZE, row_blk*BLOCK_SIZE,BLOCK_SIZE, BLOCK_SIZE)));
    rstr_scnr_blk_ptr = (rstr_scnr_blk_ptr + 1)%total_blk;
    return  rstr_scnr_blk_ptr==0;
}

int dct::get_height_blk() {return height_blk;}

int dct::get_width_blk(){return width_blk;}

int dct::get_total_blk(){total_blk;}

int dct::get_height_padded(){return height_padded;}

int dct::get_width_padded(){return width_padded;}

int dct::get_rstr_scnr_blk_ptr(){return rstr_scnr_blk_ptr;}

cv::Mat dct::get_image(){return image;}

cv::Mat dct::get_padded_image(){return padded_image;}

cv::Mat dct::get_rcvrd_image(){return rcvrd_image;}

vector<vector<tuple<int,int>>> dct::get_vect(){ return dct::vect;}





