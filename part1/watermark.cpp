//
// Watermark.cpp : Add watermark to an image, or inspect if a watermark is present.
//
// Based on skeleton code by D. Crandall, Spring 2018
//
// PUT YOUR NAMES HERE
//
//

//Link to the header file
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <SImage.h>
#include <SImageIO.h>
#include <fft.h>
#include <bitset>
#include <math.h>
#include <png.h>
#include <vector>
#define PI 3.14159265

using namespace std;

// This code requires that input be a *square* image, and that each dimension
//  is a power of 2; i.e. that input.width() == input.height() == 2^k, where k
//  is an integer. You'll need to pad out your image (with 0's) first if it's
//  not a square image to begin with. (Padding with 0's has no effect on the FT!)
//
// Forward FFT transform: take input image, and return real and imaginary parts.
//
void fft(const SDoublePlane &input, SDoublePlane &fft_real, SDoublePlane &fft_imag)
{
  fft_real = input;
  fft_imag = SDoublePlane(input.rows(), input.cols());

  FFT_2D(1, fft_real, fft_imag);
}

// Inverse FFT transform: take real and imaginary parts of fourier transform, and return
//  real-valued image.
//
void ifft(const SDoublePlane &input_real, const SDoublePlane &input_imag, SDoublePlane &output_real)
{
  output_real = input_real;
  SDoublePlane output_imag = input_imag;

  FFT_2D(0, output_real, output_imag);
}

// Write this in Part 1.1
SDoublePlane fft_magnitude(const SDoublePlane &fft_real, const SDoublePlane &fft_imag)
{
	SDoublePlane intensity = SDoublePlane(fft_real.rows(), fft_real.cols());
	int i,j;
	for(i=0;i<fft_real.rows();i++)
	{
		double *rptr = intensity[i];
		for(j=0;j<fft_real.cols();j++)
			rptr[j] = log(sqrt(pow(fft_real[i][j], 2) + pow(fft_imag[i][j], 2)));
	}
	return intensity;
}

SDoublePlane add_padding(SDoublePlane &input_image)
{
	int rows = input_image.rows();
	int cols = input_image.cols();
	int bigger = max(rows, cols);
	int count=0, i, j;
	while(bigger>0)
	{
		bigger/=2;
		count++;
	}
	int pad_size = pow(2,count);
	if(pad_size == max(rows,cols))
		return input_image;
	SDoublePlane final_image = SDoublePlane(pad_size, pad_size);
	for(i=0;i<rows;i++)
	{
		for(j=0;j<cols;j++)
		{
			final_image[i][j] = input_image[i][j];
		}
	}
	return final_image;
}

// Write this in Part 1.2
void remove_interference(const SDoublePlane &fft_real, const SDoublePlane &fft_imag)
{
        double val;
        int i,j;
        for(i=0;i<fft_real.rows();i++)
        {
                for(j=0;j<fft_real.cols();j++)
                {
                        val = log(sqrt(pow(fft_real[i][j], 2) + pow(fft_imag[i][j], 2)));
                        if(val > 0 && (i != fft_real.rows()/2 && j != fft_real.cols()/2))
                        {
                                fft_real[i][j] = 0;
                                fft_imag[i][j] = 0;
                        }
                }
        }
}

// Write this in Part 1.3 -- add watermark N to image
SDoublePlane mark_image(const SDoublePlane &input, int N)
{
	SDoublePlane changed_values = SDoublePlane(input.rows(), input.cols());
	srand(N);
	int myrand = rand()%9999997;
	string binary = bitset<128>(myrand).to_string();
	double rad;
	int h,v,x,y,x1,y1,j;
	h = input.rows()/2;
	v = input.cols()/2;
	double r = 30;
	double a = 0.008;
	int k = 0;
	int i = 0;
	for(i=0;i<input.rows();i++)
	{
		for(j=0;j<input.cols();j++)
			changed_values[i][j] = input[i][j];
	}
	
	for(rad = 270; rad > 90; rad = rad - 1.40625)
	{
			x = (r * cos(rad*PI/180))+h;
			x1 =  (r * cos((rad+180)*PI/180))+h;
			y1 =  (r * sin((rad+180)*PI/180)+v);
			y = (r * sin(rad*PI/180)+v);
			changed_values[x][y] = input[x][y] + (a*double(binary[k])*fabs(input[x][y]));
			changed_values[x1][y1] = input[x1][y1] + (a*double(binary[k])*fabs(input[x1][y1]));
			k++;
	}

	return changed_values;
}

// Write this in Part 1.3 -- check if watermark N is in image
double check_image(const SDoublePlane &input, int N)
{
	srand(N);
        int myrand = rand()%9999997;
        string binary = bitset<128>(myrand).to_string();
	double rad,avg_c,avg_v,product_of_avg,sum_of_product = 0.0,sum_of_v = 0.0,sum_of_c = 0.0,corr_coff;
        int h,v,x,y,x1,y1;
        h = input.rows()/2;
        v = input.cols()/2;
        double r = 30;
        double a = 0.002;

	double sum_v = 0.0;
	double sum_c = 0.0;

	int k = 0;
        int i = 0;
	double sum_of_nonpoweredV=0, sum_of_nonpoweredC=0;

        for(rad = 270; rad > 90; rad = rad - 1.40625)
	{
		x = (r * cos(rad*PI/180))+h;
                x1 =  (r * cos((rad+180)*PI/180))+h;
                y1 =  (r * sin((rad+180)*PI/180)+v);
                y = (r * sin(rad*PI/180)+v);
		
		sum_v = sum_v + (int(binary[i])+int(binary[i]));
		sum_c = sum_c + (input[x][y] + input[x1][y1]);
		
		sum_of_product += ((input[x][y]*int(binary[i]))+(input[x1][y1]*int(binary[i])));
		sum_of_v += (pow(input[x][y],2) + pow(input[x1][y1],2));
		sum_of_c += (pow(int(binary[i]),2) + pow(int(binary[i]),2));
		sum_of_nonpoweredV+=(input[x][y]+input[x1][y1]);
		sum_of_nonpoweredC+=(int(binary[i]) + int(binary[i]));

		i++;
	}

	i = i*2-1;
	avg_v = sum_of_nonpoweredV/i;
	avg_c = sum_of_nonpoweredC/i;
	product_of_avg = avg_c * avg_v;
	corr_coff = (sum_of_product - i*avg_c*avg_v)/((sqrt(sum_of_v-(i*pow(avg_v,2)))) * (sqrt(fabs(sum_of_c - (i*pow(avg_c,2))))));
	return corr_coff;
}


int main(int argc, char **argv)
{
  try {

    if(argc < 4)
      {
	cout << "Insufficent number of arguments; correct usage:" << endl;
	cout << "    p2 problemID inputfile outputfile" << endl;
	return -1;
      }
    
    string part = argv[1];
    string inputFile = argv[2];
    string outputFile = argv[3];
    cout << "In: " << inputFile <<"  Out: " << outputFile << endl;
    SDoublePlane red_plane, green_plane, blue_plane;
    SImageIO::read_png_file(inputFile.c_str(), red_plane, green_plane, blue_plane);
 
    if(part == "1.1")
      {
	// do something here!
	SDoublePlane paddedR_image, paddedG_image, paddedB_image;
	paddedR_image = add_padding(red_plane);
	paddedG_image = add_padding(green_plane);
	paddedB_image = add_padding(blue_plane);
	SDoublePlane realR_part, imgR_part, realG_part, imgG_part, realB_part, imgB_part;
	fft(paddedR_image, realR_part, imgR_part);
	fft(paddedG_image, realG_part, imgG_part);
	fft(paddedB_image, realB_part, imgB_part);
	SDoublePlane red_mag = fft_magnitude(realR_part, imgR_part);
	SDoublePlane green_mag = fft_magnitude(realG_part, imgG_part);
	SDoublePlane blue_mag = fft_magnitude(realB_part, imgB_part);
	SImageIO::write_png_file(outputFile.c_str(),red_mag,green_mag, blue_mag);
      }
    else if(part == "1.2")
      {
	SDoublePlane realR_part, imgR_part, realG_part, imgG_part, realB_part, imgB_part, padded_red_plane, padded_green_plane, 
	padded_blue_plane;
	padded_red_plane = add_padding(red_plane);
	padded_green_plane = add_padding(green_plane);
	padded_blue_plane = add_padding(blue_plane);
	fft(padded_red_plane, realR_part, imgR_part);
        fft(padded_green_plane, realG_part, imgG_part);
        fft(padded_blue_plane, realB_part, imgB_part);
	remove_interference(realR_part, imgR_part);
	remove_interference(realG_part, imgG_part);
	remove_interference(realB_part, imgB_part);
	SDoublePlane outputR, outputG, outputB;
	ifft(realR_part, imgR_part, outputR);
	ifft(realG_part, imgG_part, outputG);
	ifft(realB_part, imgB_part, outputB);
	SImageIO::write_png_file(outputFile.c_str(), outputR, outputG, outputB);
      }
    else if(part == "1.3")
      {
	if(argc < 6)
	  {
	    cout << "Need 6 parameters for watermark part:" << endl;
	    cout << "    p2 1.3 inputfile outputfile operation N" << endl;
	    return -1;
	  }
	int N = atoi(argv[5]);
	string op(argv[4]);
	if(op == "add")
	  {
	    // add watermark
		SDoublePlane im_r,im_r_real,im_r_imag,fin_red,op_r;
   		SDoublePlane input_image = SImageIO::read_png_file(inputFile.c_str());
		SDoublePlane padded_input_image = add_padding(input_image);
	        fft(padded_input_image,im_r_real,im_r_imag);
        	fin_red =  mark_image(im_r_real,N);
		ifft(fin_red,im_r_imag,op_r);	
		SImageIO::write_png_file(outputFile.c_str(),op_r,op_r,op_r);
	  }
	else if(op == "check")
	  {
	    // check watermark
		SDoublePlane im_r,im_r_real,im_r_imag,fin_red,op_r;
                SDoublePlane input_image = SImageIO::read_png_file(inputFile.c_str());
		SDoublePlane padded_input_image = add_padding(input_image);
                fft(padded_input_image,im_r_real,im_r_imag);
                double ans =  check_image(im_r_real,N);
		double threshold =0.01;
		if(ans > threshold)
			cout<< "Watermark exists!!";
		else
			cout<< "Watemark does not exists!!";
	  }
	else
	  throw string("Bad operation!");
       
      }
    else
      throw string("Bad part!");

  } 
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
    cout<<"New Statement";
}


