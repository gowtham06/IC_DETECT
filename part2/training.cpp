//
// Created by Gowtham Kannan on 2/6/18.
//

#include "SImage.h"
#include "SImageIO.h"
#include "edgeDetection.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>

// #include "detect.cpp"
// #include "edge_detection.h"

void appendFilePath(std::vector<std::string>& file_names, const char * file_path){
    for(int i=0;i<file_names.size();i++){
//        string a = "hello ";
//        const char *b = "world";
        string c = file_path + file_names[i];
        const char *C = c.c_str();
        file_names[i]=C;
//        cout<<"File_Path:"<<file_names[i]<<endl;
    }
}
void append_text2File(string file_name, int width,int height,double val_1,double val_2,double val_3,double val_4){
    std::ofstream outfile;
    outfile.open(file_name, std::ios_base::app);
    outfile<<width<<","<<height<<","<<val_1<<","<<val_2<<","<<val_3<<","<<val_4<<"\n";
    outfile.close();
}

void append_text2File(string file_name, int width,int height,Feature feature_point,int class_id){
//    cout<<"File append";
    std::ofstream outfile;
    outfile.open(file_name, std::ios_base::app);
    outfile<<width<<","<<height<<","<<feature_point.r_contrib<<","<<feature_point.g_contrib<<","<<feature_point.b_contrib<<","<<feature_point.edge_point_count<<","<<std::to_string(class_id)<<"\n";
    outfile.close();
}

void additional_features(string file_name,Image img,  const  SDoublePlane & edge_image,int row_index,int column_index,int width,int height){
    int count=0;
    while(count<=10){
        cout<<"Count:"<<count<<"\t";
        int start_row_index;
        int start_column_index;
        start_row_index=std::rand() % ( edge_image.rows()-height );
        start_column_index=std::rand() % ( edge_image.cols()-width );
        start_row_index=row_index;
        
        while(start_row_index==row_index || start_column_index==column_index){
//            cout<<"Loop"<<endl;
            start_row_index=std::rand() % ( edge_image.rows()-height );
            start_column_index=std::rand() % ( edge_image.cols()-width );
        }
        Feature feature_point=SImageIO::get_pixel_feature(img,edge_image,start_row_index,start_column_index,width,height);
        cout<<"ROw_index:"<<start_row_index<<","<<"Column_Index:"<<start_column_index<<endl;
        cout<<"Feature are:"<<endl;
        cout<<feature_point.r_contrib<<","<<feature_point.g_contrib<<","<<feature_point.b_contrib<<","<<feature_point.edge_point_count<<endl;
        
        
        append_text2File(file_name,width,height,feature_point,0);
        count+=1;
        
    
    }
}


void train_images(string train_feature_file){
    const char * text_data_dir="/Users/gowthamkannan/CLionProjects/chezhou-gkbandep-megpatil-spant-a1/part2/train_data/";
    const  char * image_data_dir="/Users/gowthamkannan/CLionProjects/chezhou-gkbandep-megpatil-spant-a1/part2/images_train_set/";
//    string train_feature_file="/Users/gowthamkannan/CLionProjects/chezhou-gkbandep-megpatil-spant-a1/part2/training_dat.csv";
    DIR* dirp;
    dirp=opendir(text_data_dir);
    std::vector<std::string> text_files;
    std::vector<std::string> image_files;
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL) {
        if (strstr(dp->d_name,"txt")) {
            text_files.push_back(dp->d_name);
        }
        
    }
    dirp=opendir(image_data_dir);
    while ((dp = readdir(dirp)) != NULL) {
        if (strstr(dp->d_name,"png")) {
            image_files.push_back(dp->d_name);
        }
        
    }
    std::sort(text_files.begin(), text_files.end());
    std::sort(image_files.begin(), image_files.end());
    
    appendFilePath(text_files,text_data_dir);
    appendFilePath(image_files,image_data_dir);
    
    int nosFiles=text_files.size();
    double curr_feature=1.00,prev_feature=-100.00;
    
    for(int i=0;i<text_files.size();i++){
        cout<<" For file :"<<text_files[i]<<endl<<endl;
        ifstream in(text_files[i]);
        
        if(!in) {
            cout << "Cannot open input file.\n";
        }
        else{
            std::string str;
            int row_index,column_index,height,width;
            while (std::getline(in, str)) {
                std::vector<std::string> results=split(str,' ');
                row_index=stoi(results[0]);
                column_index=stoi(results[1]);
                height=stoi(results[2]);
                width=stoi(results[3]);
                Image img=SImageIO::get_color_pixel(image_files[i].c_str());
                SDoublePlane input_image=SImageIO::read_png_file(image_files[i].c_str());
                SDoublePlane edge_image=find_edges(input_image);
                Feature feature_point=SImageIO::get_pixel_feature(img,edge_image,row_index,column_index,width,height);
                append_text2File(train_feature_file,width,height,feature_point,1);
//                additional_features(train_feature_file,img,edge_image,row_index,column_index,width,height);
            }
        }
        in.close();
    }

    }


int main(){
//    train_images();
    string file_name="/Users/gowthamkannan/CLionProjects/chezhou-gkbandep-megpatil-spant-a1/part2/";
    string image_file="/Users/gowthamkannan/CLionProjects/chezhou-gkbandep-megpatil-spant-a1/part2/images_train_set/ic_1.png";
    string train_feature_file="/Users/gowthamkannan/CLionProjects/chezhou-gkbandep-megpatil-spant-a1/part2/training_dat.csv";
    train_images(train_feature_file);
}


