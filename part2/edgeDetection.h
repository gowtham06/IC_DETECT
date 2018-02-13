//
//  edgeDetection.h
//  
//
//  Created by Gowtham Kannan on 2/8/18.
//

#ifndef edgeDetection_h
#define edgeDetection_h

#include <math.h>
#define DEG2RAD 0.017453293f
#define pi 3.1415926f
//#include <cmath.h>
// Convolution using seperable kernel
void write_image(const string &filename, const SDoublePlane &input){
    SDoublePlane output_planes[3];
    for(int p=0;p<3;p++){
        output_planes[p]=input;
    }
    SImageIO::write_png_file(filename.c_str(), output_planes[0], output_planes[1], output_planes[2]);
    
}
int reflect(int boundry,int curr_pos){
    // To tackle boundry condition using reflection
    if(curr_pos < 0)
    {
        return -1*curr_pos - 1;
    }
    if(curr_pos >= boundry)
    {
        return 2*boundry - curr_pos - 1;
    }
    return curr_pos;
}

SDoublePlane convolve_separable(const SDoublePlane &input, const SDoublePlane &row_filter, const SDoublePlane &col_filter)
{
    SDoublePlane output(input.rows(), input.cols());
    int i_1,j_1;
    int i_2,j_2;
    int temp_sum=0;
    int data,coeff;
    int row_pos=0;
    //    Row wise convolution
    for(i_1=0;i_1<input.rows();i_1++){
        for(j_1=0;j_1<input.cols();j_1++){
            j_2=-1*(row_filter.cols()/2);
            double conv_value=0.0;
            double  norm_sum=0.0;
            while (j_2<=row_filter.cols()/2){
                if((j_1+j_2>=0) and (j_1+j_2<input.cols())){
                    norm_sum=norm_sum+pow(row_filter[0][j_2+row_filter.cols()/2],2);
                    conv_value=conv_value+(row_filter[0][j_2+row_filter.cols()/2]*input[i_1][j_1+j_2]);
                }
                j_2=j_2+1;
            }
            norm_sum=sqrt(norm_sum);
            output[i_1][j_1]=conv_value;
        }
    }
    
    //    Column wise convolution
    
    for(i_1=0;i_1<input.rows();i_1++){
        for(j_1=0;j_1<input.cols();j_1++){
            i_2=-1*(col_filter.rows()/2);
            double  conv_value=0.0;
            double  norm_sum=0.0;
            while(i_2<=col_filter.rows()/2){
                if((i_1+i_2>=0) and (i_1+i_2<input.rows())){
                    norm_sum=norm_sum+pow(col_filter[i_2+col_filter.rows()/2][0],2);
                    conv_value=conv_value+(col_filter[i_2+col_filter.rows()/2][0]*output[i_1+i_2][j_1]);
                }
                i_2=i_2+1;
            }
            norm_sum=sqrt(norm_sum);
            output[i_1][j_1]=conv_value;
        }
    }
    return output;
}



// Convolve an image with a  convolution kernel
//

SDoublePlane convolve_general(const SDoublePlane &input, const SDoublePlane &filter)
{
    //  cout<<" In general convolution function";
    SDoublePlane output(input.rows(), input.cols());
    int i_1=0,j_1=0;
    int i_2=0,j_2=0;
    double norm;
    // Convolution code here
    for (i_1 = int(filter.rows())/ 2; i_1 < input.rows() - int(filter.rows() / 2); ++i_1) // iterate through image
    {
        for (j_1 = int(filter.cols() / 2); j_1 < input.cols() - int( filter.cols()/ 2); ++j_1)
        {
            int sum = 0; // sum will be the sum of input data * coeff terms
            norm=0.0;
            
            for (i_2 = - int(filter.rows() / 2); i_2 <= int(filter.rows() / 2); ++i_2) // iterate over kernel
            {
                for (j_2 = - int(filter.cols()/ 2); j_2 <= int(filter.cols() / 2); ++j_2)
                {
                    int data = input[i_1 + i_2][j_1 + j_2];
                    int coeff = filter[i_2 + int(filter.rows()/ 2)][j_2 + int( filter.cols()/ 2)];
                    norm+=pow(coeff,2);
                    
                    sum += data * coeff;
                }
            }
            //      out[i][j] = sum / scale; // scale sum of convolution products and store in output
            output[i_1][j_1]=sum/sqrt(norm);
        }
    }
    
    return output;
}


// Apply gaussian filter
SDoublePlane apply_gaussian_filter(const SDoublePlane &input){
    //  cout<<"In gaussian filter function";
    SDoublePlane output(input.rows(),input.cols());
    // Initializing the gaussian filter
    SDoublePlane gaussian_filter(3,3);
    gaussian_filter[0][0]=3,gaussian_filter[0][1]=5,gaussian_filter[0][2]=3;
    gaussian_filter[1][0]=5,gaussian_filter[1][1]=9,gaussian_filter[1][2]=5;
    gaussian_filter[2][0]=3,gaussian_filter[2][1]=5,gaussian_filter[2][2]=3;
    
    SDoublePlane H_x(3,1);
    SDoublePlane H_y(1,3);
    H_x[0][0]=1,H_x[1][0]=2,H_x[2][0]=1;
    H_y[0][0]=1,H_y[0][1]=2,H_y[0][2]=1;
    
    //  cout<<H_x.rows()<<" "<<H_x.cols()<<endl;
    //  cout<<H_y.rows()<<" "<<H_y.cols()<<endl;
    
    //  output=convolve_general(input,gaussian_filter);
    output=convolve_separable(input,H_x,H_y);
    return output;
    
}


double getAngle(double g_x,double g_y){
    return atan(g_y/g_x)*(180/pi);
}

// Apply a sobel operator to an image, returns the result
//
SDoublePlane sobel_gradient_filter(const SDoublePlane &input, bool _gx)
{
    //  cout<<"In Sobel gradient function";
    SDoublePlane output(input.rows(), input.cols());
    SDoublePlane x_vector(3,1);
    SDoublePlane y_vector(3,1);
    
    /// / Implement a sobel gradient estimation filter with 1-d filters
    if(_gx){
        x_vector[0][0]=-1,x_vector[1][0]=-2,x_vector[2][0]=-1;
        y_vector[0][0]=1,y_vector[0][1]=0,y_vector[0][2]=-1;
        SDoublePlane filter(3,3);
        filter[0][0]=-1,filter[0][1]=0,filter[0][2]=1;
        filter[1][0]=-2,filter[1][1]=0,filter[1][2]=2;
        filter[2][0]=-1,filter[2][1]=0,filter[2][2]=1;
        output=convolve_general(input,filter);
        
        //    output=convolve_separable(input,x_vector,y_vector);
    }
    else{
        x_vector[0][0]=-1,x_vector[1][0]=0,x_vector[2][0]=-1;
        y_vector[0][0]=1,y_vector[0][1]=2,y_vector[0][2]=1;
        SDoublePlane filter(3,3);
        filter[0][0]=1,filter[0][1]=2,filter[0][2]=1;
        filter[1][0]=0,filter[1][1]=0,filter[1][2]=0;
        filter[2][0]=-2,filter[2][1]=-2,filter[2][2]=-1;
        //    output=convolve_separable(input,x_vector,y_vector);
        output=convolve_general(input,filter);
    }
    
    
    return output;
}



SDoublePlane find_edges(const SDoublePlane &input, double thresh=0) {
    //  cout<<"In find edges function";
    SDoublePlane output(input.rows(), input.cols());
    
    // Implement an edge detector of your choice, e.g.
    // use your sobel gradient operator to compute the gradient magnitude and threshold
    // Applying gausian filter to the image before computing sobel gradients;
    SDoublePlane gaunsian_filtered=apply_gaussian_filter(input);
    SDoublePlane G_x=sobel_gradient_filter(input,true);
    write_image("G_x.png",G_x);
    SDoublePlane G_y=sobel_gradient_filter(input, false);
    write_image("G_y.png",G_y);
    SDoublePlane G_M(G_x.rows(),G_y.cols());
    
    // combining the gradients
    for(int i=0;i<input.rows();i++){
        for (int j=0;j<input.cols();j++){
            G_M[i][j]=sqrt(G_x[i][j]*G_x[i][j]+G_y[i][j]*G_y[i][j]);
            
        }
    }
    // Perform non maximum supression
    for (int i = 1; i < output.rows()-1; i++) {
        for (int j = 1; j < output.cols() - 1; j++) {
            if ((getAngle(G_x[i][j], G_y[i][j]) >= -22.5 && getAngle(G_x[i][j], G_y[i][j]) <= 22.5) ||
                (getAngle(G_x[i][j], G_y[i][j]) <= -157.5 && getAngle(G_x[i][j], G_y[i][j]) >= -180)) {
                if ((G_M[i][j] >= G_M[i][j + 1]) && (G_M[i][j] >= G_M[i][j - 1])) {
                    output[i][j] = G_M[i][j];
                } else
                    output[i][j] = 0;
            } else if ((getAngle(G_x[i][j], G_y[i][j]) >= 22.5 && getAngle(G_x[i][j], G_y[i][j]) <= 67.5) ||
                       (getAngle(G_x[i][j], G_y[i][j]) <= -112.5 && getAngle(G_x[i][j], G_y[i][j]) >= -157.5)) {
                if ((G_M[i][j] >= G_M[i + 1][j + 1]) && (G_M[i][j] >= G_M[i - 1][j - 1])) {
                    output[i][j] = G_M[i][j];
                } else
                    output[i][j] = 0;
            } else if ((getAngle(G_x[i][j], G_y[i][j]) >= 67.5 && getAngle(G_x[i][j], G_y[i][j]) <= 112.5) ||
                       (getAngle(G_x[i][j], G_y[i][j]) <= -67.5 && getAngle(G_x[i][j], G_y[i][j]) >= -112.5)) {
                if ((G_M[i][j] >= G_M[i + 1][j]) && (G_M[i][j] >= G_M[i - 1][j])) {
                    output[i][j] = G_M[i][j];
                } else
                    output[i][j] = 0;
            } else if ((getAngle(G_x[i][j], G_y[i][j]) >= 112.5 && getAngle(G_x[i][j], G_y[i][j]) <= 157.5) ||
                       (getAngle(G_x[i][j], G_y[i][j]) <= -22.5 && getAngle(G_x[i][j], G_y[i][j]) >= -67.5)) {
                if ((G_M[i][j] >= G_M[i + 1][j - 1]) && (G_M[i][j] >= G_M[i - 1][j + 1])) {
                    output[i][j] = G_M[i][j];
                } else
                    output[i][j] = 0;
                
                
            }
        }
    }
    return output;
}


#endif /* edgeDetection_h */
