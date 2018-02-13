/*
 * Copyright 2002-2008 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

// Hacked by D. Crandall, 9/2010

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <fstream>
#include <sstream>
#include <math.h>
using namespace std;

#define PNG_DEBUG 3
#include <png.h>

#include "SImage.h"
struct Image
{
  SDoublePlane r;
  SDoublePlane g;
  SDoublePlane b;
};

struct Feature{
    double r_contrib;
    double g_contrib;
    double b_contrib;
    double edge_point_count;
};
double cint(double x){
    if (modf(x,0)>=.5)
        return x>=0?ceil(x):floor(x);
    else
        return x<0?ceil(x):floor(x);
}

double round(double r,unsigned places){
    double off=pow(10,places);
    return cint(r*off)/off;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

class SImageIO
{
 protected:
 static  void abort_(const char * s, ...)
    {
      va_list args;
      va_start(args, s);
      vfprintf(stderr, s, args);
      fprintf(stderr, "\n");
      va_end(args);
      abort();
    }


 public:

static Image get_color_pixel(const char * file_name){
  Image img;
  SDoublePlane r,g,b;
  read_png_file(file_name,r,g,b);
  img.r=r;
  img.g=g;
  img.b=b;
  return img;
}
 // read a color image, and convert to a single grayscale plane
 static SDoublePlane read_png_file(const char *file_name)
   {
     SDoublePlane r, g, b;
     read_png_file(file_name, r, g, b);

     SDoublePlane gray(r.rows(), r.cols());
     const double *r_ptr = r[0], *g_ptr = g[0], *b_ptr = b[0];
     double *gray_ptr = gray[0];
     int sz = r.rows() * r.cols();
     for(int i=0; i<sz; i++)
       gray_ptr[i] = r_ptr[i] * 0.3 + g_ptr[i] * 0.6 + b_ptr[i] * 0.1;

     return gray;
   }

    
    static double get_pixel_feature(const SDoublePlane & input,int row_index,int column_index,int width,int height){
        //    cout<<"File_Name:"<<file_name<<endl;
        //    SDoublePlane input_image=SImageIO::read_png_file(file_name.c_str());
        //    SDoublePlane edge_image=find_edges(input_image);
        //    cout<<row_index<<" "<<column_index<<" "<<width<<" "<<height<< " "<<input_image[row_index][column_index];
        
        double pixel_sum=0.0;
        for(int i=row_index;i<=row_index+height;i++) {
            for (int j = column_index; j <= column_index + width; j++) {
                // cout<<"R:"<<img.r[i][j]<<" "<<"G:"<<img.g[i][j]<<" "<<"B:"<<img.b[i][j]<<endl;
                pixel_sum += input[i][j];
            }
        }
        return pixel_sum;
    }
    
//    function which returns the feature set of a window
    static Feature get_pixel_feature(Image img,const SDoublePlane & input,int row_index,int column_index,int width,int height,double edge_threshold=0){
        Feature feature_point;
        double r_contrib,g_contrib,b_contrib;
        double edge_point_count=0;
//        cout<<(width*height)<<endl;
        for(int i=row_index;i<=row_index+height;i++){
            for(int j=column_index;j<=column_index+width;j++){
                if((img.r[i][j]==img.g[i][j])&&(img.g[i][j]==img.b[i][j])){
                    r_contrib+=0.33;
                    g_contrib+=0.33;
                    b_contrib+=0.33;
                    
                }
                else if((img.r[i][j]==img.g[i][j]) && (img.r[i][j]!=img.b[i][j])){
                    if(img.r[i][j]>img.b[i][j]){
                    r_contrib+=0.5;
                    g_contrib+=0.5;
                    }
                    else{
                        b_contrib+=1.0;
                    }
                }
                
                else if((img.r[i][j]==img.b[i][j]) && (img.r[i][j]!=img.g[i][j])){
                    if(img.r[i][j]>img.g[i][j]){
                        r_contrib+=0.5;
                        b_contrib+=0.5;
                    }
                    else{
                        g_contrib+=1.0;
                    }
                    
                }
                else if((img.g[i][j]==img.b[i][j]) && (img.r[i][j]!=img.g[i][j])){
                    if(img.g[i][j]>img.r[i][j]){
                        b_contrib+=0.5;
                        g_contrib+=0.5;
                    }
                    else{
                        r_contrib+=1.0;
                    }
                }
                else{
                    double max=img.r[i][j] > img.g[i][j] ? (img.r[i][j] > img.b[i][j] ? img.r[i][j] : img.b[i][j]) : (img.g[i][j] > img.b[i][j] ? img.g[i][j] : img.b[i][j]) ;
                    if(img.r[i][j]==max){
                        r_contrib+=1.0;
                        
                    }
                    else if(img.g[i][j]==max){
                        g_contrib+=1.0;
                    }
                    else{
                        b_contrib+=1.0;
                    }
                }
                if(input[i][j]>=200){
                    edge_point_count+=1;
                }
            }
        }
        feature_point.r_contrib=r_contrib/(width*height);
        feature_point.g_contrib=g_contrib/(width*height);
        feature_point.b_contrib=b_contrib/(width*height);
        feature_point.edge_point_count=edge_point_count;
        return feature_point;
    }

 // read a color image, into three separate red, green, and blue color planes
 static void read_png_file(const char* file_name,  SDoublePlane &red_plane,  SDoublePlane &green_plane,  SDoublePlane &blue_plane)
    {
      int width, height;
      png_byte color_type;
      png_byte bit_depth;
      
      png_structp png_ptr;
      png_infop info_ptr;
      int number_of_passes;

      char header[8];	// 8 is the maximum size that can be checked

      /* open file and test for it being a png */
      FILE *fp = fopen(file_name, "rb");
      if (!fp)
	abort_("[read_png_file] File %s could not be opened for reading", file_name);
      fread(header, 1, 8, fp);
      if (png_sig_cmp((png_byte *)header, 0, 8))
	abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


      /* initialize stuff */
      png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
      if (!png_ptr)
	abort_("[read_png_file] png_create_read_struct failed");

      info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr)
	abort_("[read_png_file] png_create_info_struct failed");

      if (setjmp(png_jmpbuf(png_ptr)))
	abort_("[read_png_file] Error during init_io");

      png_init_io(png_ptr, fp);
      png_set_sig_bytes(png_ptr, 8);

      png_read_info(png_ptr, info_ptr);

      width  = png_get_image_width(png_ptr, info_ptr);
      height = png_get_image_height(png_ptr, info_ptr);
     // color_type = info_ptr->color_type;
      bit_depth = png_get_bit_depth(png_ptr, info_ptr);
      //      num_channels = png_get_channels(png_ptr, info_ptr);

      number_of_passes = png_set_interlace_handling(png_ptr);
      png_read_update_info(png_ptr, info_ptr);


      /* read file */
      if (setjmp(png_jmpbuf(png_ptr)))
	abort_("[read_png_file] Error during read_image");
      
      //assert (color_type == PNG_COLOR_TYPE_RGB ||
     	//      color_type == PNG_COLOR_TYPE_RGBA);

      //      const int bytes_per_pixel = info_ptr->rowbytes / width;
      const int bytes_per_pixel = png_get_rowbytes(png_ptr, info_ptr) / width;
      assert(bit_depth == 8);

      _DTwoDimArray<unsigned char> img(height, width*bytes_per_pixel);
      //      row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
      //      for (y=0; y<height; y++)
      //	row_pointers[y] = (png_byte*) malloc(info_ptr->rowbytes);

      png_read_image(png_ptr, img.row_pointers());

      // collapse to grayscale and convert to double
      red_plane = SDoublePlane(height, width);
      green_plane = SDoublePlane(height, width);
      blue_plane = SDoublePlane(height, width);
      for(int i=0; i<height; i++)
	{
	  const unsigned char *img_cp = img[i];
	  double *r_ptr = red_plane[i], *g_ptr = green_plane[i], *b_ptr = blue_plane[i];
	  for(int j=0, j2=0; j<width; j++, j2+=bytes_per_pixel)
	    {
	      r_ptr[j] = img_cp[j2];
	      g_ptr[j] = img_cp[j2+1];
	      b_ptr[j] = img_cp[j2+2];
	    }
	}
      fclose(fp);
    }


 static  void write_png_file(const char* file_name, const SDoublePlane &img_r, const SDoublePlane &img_g, const SDoublePlane &img_b)
    {
      int width, height;
      png_byte color_type;
      png_byte bit_depth;
      
      png_structp png_ptr;
      png_infop info_ptr;
      int number_of_passes;

      bit_depth=8;
      color_type=PNG_COLOR_TYPE_RGB;

      /* create file */
      FILE *fp = fopen(file_name, "wb");
      if (!fp)
	abort_("[write_png_file] File %s could not be opened for writing", file_name);

      width = img_r.cols();
      height = img_r.rows();

      /* initialize stuff */
      png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
      if (!png_ptr)
	abort_("[write_png_file] png_create_write_struct failed");

      info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr)
	abort_("[write_png_file] png_create_info_struct failed");

      if (setjmp(png_jmpbuf(png_ptr)))
	abort_("[write_png_file] Error during init_io");

      png_init_io(png_ptr, fp);


      /* write header */
      if (setjmp(png_jmpbuf(png_ptr)))
	abort_("[write_png_file] Error during writing header");

      png_set_IHDR(png_ptr, info_ptr, width, height,
		   bit_depth, color_type, PNG_INTERLACE_NONE,
		   PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

      png_write_info(png_ptr, info_ptr);


      /* write bytes */
      if (setjmp(png_jmpbuf(png_ptr)))
	abort_("[write_png_file] Error during writing bytes");

      _DTwoDimArray<unsigned char> img(height, width*3);
      for(int i=0; i<height; i++)
	{
	  unsigned char *out_row_ptr = img[i];
	  double *r_ptr = img_r[i], *g_ptr = img_g[i], *b_ptr = img_b[i];

	  for(int j=0, j2=0; j<width; j++, j2+=3)
	    {
	      out_row_ptr[j2] = (unsigned char)r_ptr[j];
	      out_row_ptr[j2+1] = (unsigned char)g_ptr[j];
	      out_row_ptr[j2+2] = (unsigned char)b_ptr[j];
	    }
	}

      png_write_image(png_ptr, img.row_pointers());

      /* end write */
      if (setjmp(png_jmpbuf(png_ptr)))
	abort_("[write_png_file] Error during end of write");

	  


      png_write_end(png_ptr, NULL);

      /* cleanup heap allocation */
      /*      for (y=0; y<height; y++)
	free(row_pointers[y]);
      free(row_pointers);
      */
      fclose(fp);
    }

};

