//
// Created by diego on 29/12/19.
//

#include "dct.h"

dct::dct(int height, int width){
    height_blk = ceil((float) height/BLOCK_SIZE);
    width_blk  = ceil((float) width/BLOCK_SIZE);
    total_blk  = height_blk * width_blk;

    vector<int> a(BLOCK_SIZE*BLOCK_SIZE,0);
    vector< tuple<vector<int>, int>> tmp(total_blk,std::make_tuple(a,0));

    height_padded = height_blk * BLOCK_SIZE;
    width_padded  = width_blk * BLOCK_SIZE;

    padded_image.create(height_padded, width_padded, 0);
    rcvrd_image.create(height_padded, width_padded, 0);
    rstr_scnr_blk_ptr = 0;
}

dct::dct(cv::Mat image) : dct(image.rows, image.cols) {
    this->image = image;
    image.copyTo(padded_image(cv::Rect(0,0,image.cols, image.rows)));
    padded_image.convertTo(padded_image, CV_32F);
}

void dct::dct_quant_rle_frame(){

    int init_row_idx, init_col_idx;
    cv::Mat tmp_block, dct_block, quant_block;
    vector<int> tmp_vct;


    for(int i = 0; i < height_blk; i++){
        init_row_idx = i*BLOCK_SIZE;

        for(int j = 0; j< width_blk; j++){
            init_col_idx = j*BLOCK_SIZE;

            tmp_block = padded_image(cv::Rect(init_col_idx,init_row_idx, BLOCK_SIZE, BLOCK_SIZE));
            cv::dct(tmp_block, dct_block);
            dct_block.convertTo(dct_block,CV_16S);
            divide(dct_block,quant_mat, quant_block);

            zigzag z = zigzag(quant_block);
            tmp_vct = z.load_zigzag();

            rle r = rle(tmp_vct);
            tmp_vct = r.rm_fin_zeros();

            vect[rstr_scnr_blk_ptr++] = make_tuple(tmp_vct,r.get_num_zeros());
        }
    }
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

vector< tuple<vector<int>, int>> dct::get_vect(){ return dct::vect;}




