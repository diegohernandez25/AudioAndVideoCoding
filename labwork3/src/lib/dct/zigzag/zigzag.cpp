//
// Created by diego on 26/12/19.
//

#include "zigzag.h"

zigzag::zigzag(cv::Mat mat){
    this->mat = mat;
    this->rows=mat.rows;
    this->cols=mat.cols;
}

zigzag::zigzag(vector<short> vect, int rows, int cols){
    this->vect = vect;
    this->rows=rows;
    this->cols=cols;
}

vector<short> zigzag::load_zigzag(){
    if(rows==cols) return this->load_zigzag_sqr();
    else if(rows>cols) return this->load_zigzag_m_n();
    else return this->load_zigzag_n_m();
}

cv::Mat zigzag::inverse_zigzag(){
    if(rows==cols) return this->inverse_zigzag_sqr();
    else if(rows>cols) return this->inverse_zigzag_m_n();
    else return this->inverse_zigzag_n_m();
}

vector<short> zigzag::load_zigzag_sqr() {
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

vector<short> zigzag::load_zigzag_n_m() {
    int size=rows*cols, col_i=0, row_i=0, count=0, n_iter=0, direction=0;
    vector<short> output(size, 0);
    bool change = true;

    output[count++] = mat.at<short>(row_i,col_i);
    while(true){
        if(change){
            output[count++]=mat.at<short>(row_i,++col_i);
            n_iter=col_i;
            direction=1;
        }
        else{
            if(row_i==rows-1) break;
            output[count++]=mat.at<short>(++row_i,col_i);
            n_iter = row_i;
            direction = -1;
        }
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            output[count++] = mat.at<short>(row_i,col_i);
        }
        change=!change;
    }
    change=!change;
    while(true){
        if(change){direction = -1;}
        else{
            if(col_i==cols-2){
                output[count++] = mat.at<short>(row_i,++col_i);
                break;
            }
            direction = 1;
        }
        output[count++]=mat.at<short>(row_i,++col_i);
        for(int i=0; i<rows-1; i++){
            row_i+=direction; col_i-=direction;
            output[count++] = mat.at<short>(row_i,col_i);
        }
        change=!change;
    }
    change=!change;
    n_iter=rows;
    while(n_iter--!=0){
        direction = change?1:-1;
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            output[count++] = mat.at<short>(row_i,col_i);
        }
        (direction==1)?col_i++:row_i++;
        output[count++] = mat.at<short>(row_i,col_i);
        change=!change;
    }
    return output;
}

vector<short> zigzag::load_zigzag_m_n() {
    int size=rows*cols, col_i=0, row_i=0, count=0, n_iter=0, direction=0;
    vector<short> output(size, 0);
    bool change = true;

    output[count++] = mat.at<short>(row_i,col_i);
    while(true){
        if(change){
            if(col_i==cols-2){
                output[count++] = mat.at<short>(row_i,++col_i);
                break;
            }
            output[count++]=mat.at<short>(row_i,++col_i);
            n_iter=col_i;
            direction=1;
        }
        else{
            output[count++]=mat.at<short>(++row_i,col_i);
            n_iter = row_i;
            direction = -1;
        }
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            output[count++] = mat.at<short>(row_i,col_i);
        }
        change=!change;
    }
    while(true){
        direction = change?1:-1;
        for(int i=0; i<cols-1; i++){
            row_i+=direction; col_i-=direction;
            output[count++] = mat.at<short>(row_i,col_i);
        }
        if(row_i==rows-1)break;
        else output[count++] = mat.at<short>(++row_i,col_i);
        change=!change;
    }
    n_iter=cols-1;
    while(n_iter--!=0){
        if(change){
            output[count++] = mat.at<short>(row_i,++col_i);
            direction = -1;
        } else{
            output[count++] = mat.at<short>(++row_i, col_i);
            direction = 1;
        }
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            output[count++] = mat.at<short>(row_i,col_i);
        }
        change=!change;
    }
    return output;
}

cv::Mat zigzag::inverse_zigzag_m_n() {
    int col_i=0, row_i=0, count=0, n_iter=0, direction=0;
    cv::Mat res = cv::Mat(rows,cols, CV_16S);
    bool change = true;

    res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
    while(true){
        if(change){
            if(col_i==cols-2){
                res.at<short>(row_i,++col_i) = zigzag::vect.at(count++);
                break;
            }
            res.at<short>(row_i,++col_i) = zigzag::vect.at(count++);
            n_iter=col_i;
            direction=1;
        }
        else{
            res.at<short>(++row_i,col_i) = zigzag::vect.at(count++);
            n_iter = row_i;
            direction = -1;
        }
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
        }
        change=!change;
    }
    while(true){
        direction = change?1:-1;
        for(int i=0; i<cols-1; i++){
            row_i+=direction; col_i-=direction;
            res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
        }
        if(row_i==rows-1)break;
        else {
            res.at<short>(++row_i,col_i) = zigzag::vect.at(count++);
        }
        change=!change;
    }
    n_iter=cols-1;
    while(n_iter--!=0){
        if(change){
            res.at<short>(row_i,++col_i) = zigzag::vect.at(count++);
            direction = -1;
        } else{
            res.at<short>(++row_i,col_i) = zigzag::vect.at(count++);
            direction = 1;
        }
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
        }
        change=!change;
    }
    return res;
}


cv::Mat zigzag::inverse_zigzag_n_m(){
    int col_i=0, row_i=0, count=0, n_iter=0, direction=0;
    cv::Mat res = cv::Mat(rows,cols, CV_16S);
    bool change = true;

    res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
    while(true){
        if(change){
            res.at<short>(row_i,++col_i) = zigzag::vect.at(count++);
            n_iter=col_i;
            direction=1;
        }
        else{
            if(row_i==rows-1) break;
            res.at<short>(++row_i,col_i) = zigzag::vect.at(count++);
            n_iter = row_i;
            direction = -1;
        }
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
        }
        change=!change;
    }
    change=!change;
    while(true){
        if(change){direction = -1;}
        else{
            if(col_i==cols-2){
                res.at<short>(row_i,++col_i) = zigzag::vect.at(count++);
                break;
            }
            direction = 1;
        }
        res.at<short>(row_i,++col_i) = zigzag::vect.at(count++);
        for(int i=0; i<rows-1; i++){
            row_i+=direction; col_i-=direction;
            res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
        }
        change=!change;
    }
    change=!change;
    n_iter=rows;
    while(n_iter-1>0){
        n_iter--;
        direction = change?1:-1;
        for(int i=0; i<n_iter; i++){
            row_i+=direction; col_i-=direction;
            res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
        }
        (direction==1)?col_i++:row_i++;
        res.at<short>(row_i,col_i) = zigzag::vect.at(count++);
        change=!change;
    }
    return res;
}

cv::Mat zigzag::inverse_zigzag_sqr(){
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

int zigzag::get_rows(){return rows;}
int zigzag::get_cols(){return cols;}
cv::Mat zigzag::get_mat(){return zigzag::mat;}
vector<short> zigzag::get_vect(){return zigzag::vect;}