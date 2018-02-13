# a1-master
Part 1
1.1
First, we padded the image if the image was no a square. We padded it so that its rows and columns become a multiple of some power of 2.
Then, we took the fast Fourier transform to find the real and the imaginary part of the image for all red, green and blue pixel. We then found the magnitude of all the red, green and blue using the formula: E(i; j) = log(R(i; j)2 + J(i; j)2). Then we convert all the three magnitudes to get a png image.
This represents the spectrogram of the original image.

Use the following to run the code
./watermark 1.1 bike.png bike_output.png


1.2
First, we padded the image if the image was no a square. We padded it so that its rows and columns become a multiple of some power of 2.
Then, we took the fast Fourier transform to find the real and the imaginary part of the image for all red, green and blue pixel.
Then we removed the noise from the real and the imaginary part of the image. We did this by changing the real and the imaginary values of all the pixels 
whose magnitude as calculated by the formula: E(i; j) = log(R(i; j)2 + J(i; j)2) to zero except for the centre pixels whose value is kept intact.
After taking the inverse Fourier transform of the image we get the image values in the spatial domain which are then plotted onto a png image.

Use the following to run the code
./watermark 1.2 noise1.png correctednoise1.png

1.3
For this part as per the instructions we first found out the binary vector. Then added the watermark on the image. For doing this we used the given formula for making the changes in the bins. To find out the circular bins, firstly we calculated the center of the given image that is by dividing the row and column size by 2 and getting the center and stating it as h and v. Then we calculated the x and y values as x = r*cos(theta) +h and y = r*sin(theta) + v and similarly we calculated the symmetric values x1 and y1 by making an addition of PI to the given theta. Here we used 128 bits that is our L is 128 bits. 

However, while doing this we observed that for different sizes of the image we had to modify the value of alpha. For instance, for an image of size 128*128 we used alpha value as 0.008, while if we used the same value for an image of size 256*256 them the image was distorted so we had to reduce the value of alpha. But doing that and keeping the value of radius as 30cwe got respectable values.

For Checking whether there is watermark present or not we used pearson's coefficient correlation formula. We found out that if we set the threshold above 0.01 then we are getting a watermarked image while any non-watermarked image has a negative coefficient correlation.
We used the following formula

Pearson Correlation Coefficient Formula:
r = (Summation of Xi * Yi - #ofElements * Mean of X * Mean of Y) / (SquareRoot(Summation of Square of Xi - #ofElements * Square of the Mean of X) + (SquareRoot(Summation of Square of Yi - #ofElements * Square of the Mean of Y))

Use the following to run the code
./waretmark 1.3 [pen.png] [output.png] add 123
./waretmark 1.3 [pen.png] [output.png] check 123


Ref: https://wikimedia.org/api/rest_v1/media/math/render/svg/01d103c10e6d4f477953a9b48c69d19a954d978a




Part 2:-
(Edge detection and IC recongnition)

2.1  & 2.2 :- to implement general convolution using 2-d kernel 
convolve_general_wrapper() in detect.cpp :- wrapper function for convolve_general() in edgeDetection.h
convolve_separable_wrapper() in detect.cpp :-wrapper function for convolve_separable() in edgeDetection.h
apply_gaussian_filter_wrapper() in detect.cpp :- wrapper function for apply_gaussian_filter() in edgeDetection.h

convolve_general() in edgeDetection.h - implements convolution using 2-d kernel. Note :- Boundry pixels are approximated to 0 in this case.

convolve_seperable() in edgeDetection.h - implements convolution using 2 seperate 1-d kernels. Note :- Boundry pixels are approximated to the pixels that fit the kernel and are normalized correspondingly.

2.3 :- To implement edge detection 
Implemented sobel Gradient method and non-maximal supression to perform edge detection on the image.
EdgeDetection.h contains all the functions that are needed for edge detection.
find_edges() in EdgeDetection.h  :- function which implements edge detection using sobel gardient. 3X3 sobel kernel is used for edge detection here.

2.4 :- To detect ICS in an image :-

HoughTranform :- houghTransform() in detect.cpp implements hough tranform on the image and GetLines() in detect.cpp detects lines in the image using hough transform 

To find the ics in a given image , a modified version of supervised learning is used. 
To train , given information regarding the postion of ics in each images are learnt. Feaures like r_contrib(red pixel contribution), g_contrib(green pixel contrib),b_contrib(blue pixel contrib) and edge_pixels present in the given areas of pre-known ics are calculated and stored in a file. These features are further normalized so as to reduce the effect of size of each ics. To calculate edge pixel count, a threhold of 220 is used (i.e) a pixel value of greater than 220 in edge_image patrix is considered as edge pixel.

Training.cpp - contains all the functions needed for training of images. For this run , all the images should be present in sub folder "imageTrainSet" and all the information regarding the position of ICS should be present in sub folder "train_data".
 
 Additionaly it does not require any command line arguments to run this file.
 
 So before executing detect.cpp, Training.cpp should be executed.
 
 detect_ics() in Detect.cpp :- This function then searches for pre-trained IC patterns using consine similarity in sliding window manner. Further to ignore the areas where ICS were already found, check_history() is used. This function checks if there were any ICS detected in a particular region and that particular region is skipped for search correspondingly.



