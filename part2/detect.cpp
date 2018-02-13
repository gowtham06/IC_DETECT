//
// detect.cpp : Detect integrated circuits in printed circuit board (PCB) images.
//
// Based on skeleton code by D. Crandall, Spring 2018
//
// PUT YOUR NAMES HERE
//
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
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <cstring>

// #include "training.cpp"


using namespace std;

// The simple image class is called SDoublePlane, with each pixel represented as
// a double (floating point) type. This means that an SDoublePlane can represent
// values outside the range 0-255, and thus can represent squared gradient magnitudes,
// harris corner scores, etc. 
//
// The SImageIO class supports reading and writing PNG files. It will read in
// a color PNG file, convert it to grayscale, and then return it to you in 
// an SDoublePlane. The values in this SDoublePlane wi be in the range [0,255].
//
// To write out an image, call write_png_file(). It takes three separate planes,
// one for each primary color (red, green, blue). To write a grayscale image,
// just pass the same SDoublePlane for all 3 planes. In order to get sensible
// results, the values in the SDoublePlane should be in the range [0,255].
//

// Below is a helper functions that overlays rectangles
// on an image plane for visualization purpose. 

// Draws a rectangle on an image plane, using the specified gray level value and line width.
//
void overlay_rectangle(SDoublePlane &input, int _top, int _left, int _bottom, int _right, double graylevel, int width)
{
  for(int w=-width/2; w<=width/2; w++) {
    int top = _top+w, left = _left+w, right=_right+w, bottom=_bottom+w;

    // if any of the coordinates are out-of-bounds, truncate them 
    top = min( max( top, 0 ), input.rows()-1);
    bottom = min( max( bottom, 0 ), input.rows()-1);
    left = min( max( left, 0 ), input.cols()-1);
    right = min( max( right, 0 ), input.cols()-1);
      
    // draw top and bottom lines
    for(int j=left; j<=right; j++)
	  input[top][j] = input[bottom][j] = graylevel;
    // draw left and right lines
    for(int i=top; i<=bottom; i++)
	  input[i][left] = input[i][right] = graylevel;
  }
}

// DetectedBox class may be helpful!
//  Feel free to modify.
//
class DetectedBox {
public:
  int row, col, width, height;
  double confidence;
};

// Function that outputs the ascii detection output file
void  write_detection_txt(const string &filename, const vector<DetectedBox> &ics)
{
  ofstream ofs(filename.c_str());

  for(vector<DetectedBox>::const_iterator s=ics.begin(); s != ics.end(); ++s)
    ofs << s->row << " " << s->col << " " << s->width << " " << s->height << " " << s->confidence << endl;
}

// Function that outputs a visualization of detected boxes
void  write_detection_image(const string &filename, const vector<DetectedBox> &ics, const SDoublePlane &input)
{
  SDoublePlane output_planes[3];

  for(int p=0; p<3; p++)
    {
      output_planes[p] = input;
      for(vector<DetectedBox>::const_iterator s=ics.begin(); s != ics.end(); ++s)
	overlay_rectangle(output_planes[p], s->row, s->col, s->row+s->height-1, s->col+s->width-1, p==2?255:0, 2);
    }

  SImageIO::write_png_file(filename.c_str(), output_planes[0], output_planes[1], output_planes[2]);

}

// Function to write the edges of the image


// The rest of these functions are incomplete. These are just suggestions to 
// get you started -- feel free to add extra functions, change function
// parameters, etc.


//
SDoublePlane convolve_general_wrapper(const SDoublePlane &input, const SDoublePlane &filter){
    return convolve_general(input,filter);
}

SDoublePlane convolve_separable_wrapper(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter){
    return convolve_separable(input,row_filter,col_filter);
}

SDoublePlane apply_gaussian_filter_wrapper(const SDoublePlane &input){
    return apply_gaussian_filter(input);
}

SDoublePlane sobel_gradient_filter_wrapper(const SDoublePlane &input, bool _gx){
    return sobel_gradient_filter(input, _gx);
}

SDoublePlane find_edges_wrapeer(const SDoublePlane &input, double thresh=0.0){
    return find_edges(input, thresh);
}

void print_mat(SDoublePlane mat){
    for(int i=0;i<mat.rows();i++){
        for(int j=0;j<mat.cols();j++){
            cout<<mat[i][j]<<" ";
        }
        cout<<endl;
    }
}

// Write the image matrix to file
void write_mat2file(string file_name,SDoublePlane mat){
    FILE * (file);
    const char* s = file_name.c_str();
    file=fopen(s,"w");
    for (int i=0;i<mat.rows();i++)
    {
        for (int j=0;j<mat.cols();j++)
        {
            fprintf (file, "%f",mat[i][j]);
            fprintf(file," ");
        }
        fprintf (file,"\n");
    }
    fclose (file);
}

SDoublePlane houghTransform(const SDoublePlane &input, double thresh=0 ){
//  SDoublePlane output(input.rows(),input.cols());

    double hough_h = ((sqrt(2.0) * (double)(input.cols()>input.rows()?input.rows():input.cols())) / 2.0);
    int accum_h=ceil(hough_h*2.0);
    int accum_w=180;
    SDoublePlane accumulator(accum_h,accum_w);
    double center_x = (double)input.rows()/2;
    double center_y = (double)input.cols()/2;

    int i=0,j=0;
//    int nos_rows=input.rows(),nos_columns=input.cols();
    double edge_count=0.0;
    for(i=0;i<input.rows();i++) {
        for (j = 0; j <input.cols(); j++) {
//            Check for edge pixel with threshold intensity 100
            if (input[i][j] > 100) {
                edge_count++;
                for(int t=0;t<180;t++){
//                        Computing the hough transform of each edge pixel
                double r = (((double) i - center_x) * cos((double) t * DEG2RAD)) +
                           (((double) j - center_y) * sin((double) t * DEG2RAD));
//                cout << "Pixel :(" << "I=" << i << ",J=" << j << ",R=" << r << ")" << endl;
                    accumulator[int(r+hough_h)][t]++;
                }
            }
        }
    }

//    cout<<"Edge Count:"<<edge_count;
      write_mat2file("h_mat.txt",accumulator);

return accumulator;
}

//Function to find the lines from the hough accumulator
std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > GetLines(const SDoublePlane & accumulator ,int threshold, int img_width,int img_height){
//    Declaring the line data structure to store the line segments
    std::vector< std::pair< std::pair<int, int>, std::pair<int, int> >  >lines;
    for(int r=0;r<accumulator.rows();r++){
        for(int c=0;c<accumulator.cols();c++){
            if(int(accumulator[r][c])>=threshold){
                int max=accumulator[r][c];
                for(int ly=-4;ly<=4;ly++){
                    for(int lx=-4;lx<=4;lx++){
                        if((ly+r>=0&& ly+r<accumulator.rows()) && (lx+c>=0 && lx+c<accumulator.cols())){
                            if(int(accumulator[r+ly][c+lx]>max)){
                                max=accumulator[r+ly][c+lx];
                                ly=lx=5;
                            }
                        }
                    }
                }
              if(max>int(accumulator[r][c]))
                  continue;
                int x_1=0,y_1=0,x_2=0,y_2=0;

                if(c >= 45 && c <= 135)
                {
                    //y = (r - x cos(t)) / sin(t)
                    x_1 = 0;
                    y_1 = ((double)(r-(accumulator.cols()/2)) - ((x_1 - (img_width/2) ) * cos(c * DEG2RAD))) / sin(c * DEG2RAD) + (img_height / 2);
                    x_2 = img_width - 0;
                    y_2 = ((double)(r-(accumulator.cols()/2)) - ((x_2 - (img_width/2) ) * cos(c * DEG2RAD))) / sin(c * DEG2RAD) + (img_height / 2);
                }
                else
                {
                    //x = (r - y sin(t)) / cos(t);
                    y_1 = 0;
                    x_1 = ((double)(r-(accumulator.cols()/2)) - ((y_1 - (img_height/2) ) * sin(c * DEG2RAD))) / cos(c * DEG2RAD) + (img_width / 2);
                    y_2 = img_height - 0;
                    x_2 = ((double)(r-(accumulator.cols()/2)) - ((y_2 - (img_height/2) ) * sin(c * DEG2RAD))) / cos(c * DEG2RAD) + (img_width/ 2);
                }

                lines.push_back(std::pair< std::pair<int, int>, std::pair<int, int> >(std::pair<int, int>(x_1,y_1), std::pair<int, int>(x_2,y_2)));


            }

        }
    }
    std::cout<<"Lines : "<<lines.size()<< " "<<threshold<<std::endl;
    return lines;
}


 DetectedBox check_history(vector<DetectedBox> ics,int x,int y){
  // function to check if a particular area has already been covered
    
  bool check_flag=false;
  DetectedBox s;
  s.row=-1;
  s.col=-1;
  s.height=-1;
  s.width=-1;
  for(int i=0;i<ics.size();i++){
      if((x>=ics[i].row) && (ics[i].row+ics[i].height>=x)){
        if((y>=ics[i].col)&&(ics[i].col+ics[i].width>=y)){
        s.row=ics[i].row;
        s.col=ics[i].col;
        s.width=ics[i].width;
        s.height=ics[i].height;
        break;
        }
        }
        else if((y>=ics[i].col)&&(ics[i].col+ics[i].width>=y)){
            if((x>=ics[i].row) && (ics[i].row+ics[i].height>=x)){
                s.row=ics[i].row;
                s.col=ics[i].col;
                s.width=ics[i].width;
                s.height=ics[i].height;
                break;
        
            }
      }
    
}
return s;
}
double get_similarity_score(Feature curr_ic,Feature curr_point){
  // function to return the cosine similarity between two feature vectors
    double dot_sum=(curr_ic.r_contrib*curr_point.r_contrib)+(curr_ic.g_contrib*curr_point.g_contrib)+(curr_ic.b_contrib*curr_point.b_contrib)+(curr_ic.edge_point_count*curr_point.edge_point_count);
    double mag_A=sqrt(pow(curr_ic.r_contrib,2)+pow(curr_ic.g_contrib,2)+pow(curr_ic.b_contrib,2)+pow(curr_ic.edge_point_count,2));
    double mag_B=sqrt(pow(curr_point.r_contrib,2)+pow(curr_point.g_contrib,2)+pow(curr_point.b_contrib,2)+pow(curr_point.edge_point_count,2));
    return (dot_sum)/(mag_A*mag_B);
    
}

vector<DetectedBox> detect_ics(Image img,const SDoublePlane & input,string train_data_file){
  // function which returns the detected ics
    vector<DetectedBox> ics;

//    std::ifstream infile(train_data_file);
    int height,width;
    int box_count=0;
    double feature=0.0;
    double fe_1,fe_2,fe_3,fe_4;
    int line_count=0;
    ifstream in(train_data_file);
    
    if(!in) {
        cout << "Cannot open input file.\n";
    }
    else{
        std::string str;
        
        int width,height;
        while (std::getline(in, str)) {
            std::vector<std::string> results=split(str,',');
            Feature curr_ic;
            width=stoi(results[0]);
            height=stoi(results[1]);
            curr_ic.r_contrib=stod(results[2]);
            curr_ic.g_contrib=stod(results[3]);
            curr_ic.b_contrib=stod(results[4]);
            curr_ic.edge_point_count=stod(results[5]);
            for(int i=50;i<input.rows()-height;i++){

                for(int j=227;j<input.cols()-width;j++){
                    cout<<"("<<i<<","<<j<<")"<<endl;
                    DetectedBox s=check_history(ics,i,j);
                    // cout<<s.row<<" sd"<<endl;
                    if(s.row!=-1){
                      j=s.col+s.width+1;
                    }
                    else{
                    
                    Feature curr_point=SImageIO::get_pixel_feature(img,input,i,j,width,height);
                    double sim_score=get_similarity_score(curr_ic,curr_point);
//                        cout<<"SImageIO::get_pixel_feature(img,input,"<<i<<","<<j<<","<<width<<","<<height<<")"<<endl;
                        // cout<<"Feature:("<<curr_point.r_contrib<<","<<curr_point.g_contrib<<","<<curr_point.b_contrib<<","<<curr_point.edge_point_count<<")";
                        
                        // cout<<"SIM_SCORE:"<<sim_score<<endl;
                    if(sim_score>=1){
                        cout<<"Found IC"<<endl;
                        DetectedBox new_ic;
                        new_ic.width=width;
                        new_ic.height=height;
                        new_ic.row=i;
                        new_ic.col=j;
                        new_ic.confidence=sim_score;
                        ics.push_back(new_ic);
                        


                    }
                    
                    
                }
                
              
            }

                
            
        }
    }
    
    }
    return ics;
  }

int main(int argc, char *argv[])
{

  if(!(argc == 2))
    {
      // string train_feature_file=train_images();
      // cout<<"finished training"<<endl;
      cerr << "usage: " << argv[0] << " input_image" << endl;
      

      return 1;
    }
    string input_filename(argv[1]);
    SDoublePlane input_image= SImageIO::read_png_file(input_filename.c_str());
     Image img=SImageIO::get_color_pixel(input_filename.c_str());
    // double feature=get_pixel_feature(img,50,227,155,152);
    // cout<<"Feauure:"<<feature<<endl;

//    write_mat2file("input_mat.txt",input_image);
    SDoublePlane edge_image=find_edges_wrapeer(input_image,0);
    write_image("edge.png",edge_image);
//    write_mat2file("output_mat.txt",edge_image);

    string train_feature_file="/Users/gowthamkannan/CLionProjects/chezhou-gkbandep-megpatil-spant-a1/part2/training_dat.csv";
  //   // string train_feature_file=train_images();
    

    vector<DetectedBox> ics=detect_ics(img,edge_image,train_feature_file);
    write_detection_txt("detected.txt", ics);
  write_detection_image("detected.png", ics, input_image);
    
//    Feature feature_1;
//
//    feature_1.r_contrib=0.786616;
//    feature_1.g_contrib=0.194511;
//    feature_1.b_contrib=0.0317772;
//    feature_1.edge_point_count=187;
//
//    Feature curr_point=SImageIO::get_pixel_feature(img,edge_image,50,227,155,152);
//    cout<<"Feature_1:("<<feature_1.r_contrib<<","<<feature_1.g_contrib<<","<<feature_1.b_contrib<<","<<feature_1.edge_point_count<<")";
//    cout<<"Feature_2:("<<curr_point.r_contrib<<","<<curr_point.g_contrib<<","<<curr_point.b_contrib<<","<<curr_point.edge_point_count<<")";
//
//     cout<<"S_Score: "<<get_similarity_score(feature_1,curr_point)<<endl;
    
//
}
