//
// Created by diego on 26/12/19.
//

#include "zigzag.h"

zigzag::zigzag(cv::Mat mat){
    zigzag::mat = mat;
}

zigzag::zigzag(vector<short> vect){
    zigzag::vect = vect;
}

cv::Mat zigzag::get_mat(){
    return zigzag::mat;
}

vector<short> zigzag::get_vect(){
    return zigzag::vect;
}

vector<short> zigzag::load_zigzag() {
    int rows=mat.rows;
    int cols=mat.cols;

    assert(rows==cols);

    int size=rows*cols;
    vector<short> output(size, 0);

    int r=0, c=0, count=0, transactions=0, n_iter, direction;
    while(count < size){
        if(transactions<rows){
            n_iter = transactions;
            for(int i=0; i<n_iter+1; i++){
                if(i!=0){
                    direction   = (transactions%2)? -1:1;
                    r = r - direction;
                    c = c + direction;
                }
                output[count++] = mat.at<short>(r,c);
            }

            transactions++;
            if(transactions< rows)
                (transactions%2==0)? r++:c++;
            else
                c++;
        }
        else{

            n_iter = rows - (transactions % rows) -1;
            for(int i=0; i<n_iter; i++){
                if(i!=0){
                    direction   = (transactions%2)? 1:-1;
                    r = r + direction;
                    c = c - direction;
                }
                output[count++] = mat.at<short>(r,c);
            }
            transactions++;
            (transactions%2==0)? c++:r++;
        }
    }

    return output;
}

cv::Mat zigzag::inverse_zigzag(){
    int rows = int(sqrt(vect.size()));
    int cols = rows;
    int size=rows*cols;

    cv::Mat res = cv::Mat(rows,cols, CV_16S);
    res = 0;
    
    int r=0, c=0, count=0, transactions=0, n_iter, direction;
    while(count < size){
        if(transactions<rows){
            n_iter = transactions;
            for(int i=0; i<n_iter+1; i++){
                if(i!=0){
                    direction   = (transactions%2)? -1:1;
                    r = r - direction;
                    c = c + direction;
                }
                res.at<short>(r,c) = zigzag::vect.at(count++);
            }

            transactions++;
            if(transactions< rows)
                (transactions%2==0)? r++:c++;
            else
                c++;
        }
        else{
            n_iter = rows - (transactions % rows) -1;
            for(int i=0; i<n_iter; i++){
                if(i!=0){
                    direction   = (transactions%2)? 1:-1;
                    r = r + direction;
                    c = c - direction;
                }
                res.at<short>(r,c) = zigzag::vect.at(count++);
            }
            transactions++;
            (transactions%2==0)? c++:r++;
        }
    }
    return res;
}
