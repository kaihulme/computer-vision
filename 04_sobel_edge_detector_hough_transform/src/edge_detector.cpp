// header inclusion
#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        
#include <opencv/highgui.h>   
#include <opencv/cxcore.h>    

using namespace cv;

void SobelEdgeDetector(cv::Mat &input, 
				       int size,
	                   cv::Mat &sobel_output);

void GaussianBlur(cv::Mat &input, 
				  int size,
	              cv::Mat &blurred_output);

int main( int argc, char** argv ) {

	// get image name
	string usr_img;
	std::cout << "\nPlease enter a image name: ";
  	std::cin >> usr_img;
	const string imageName = "resources/" + usr_img + ".png";

	// read image data
 	Mat image;
 	image = imread( imageName, 1 );
 	if(!image.data ) {
   		printf( "\nERROR: image not found!\n\n" );
   		return -1;
 	}

	// get filter type
	string usr_filter;
	std::cout << "Filter type (sobel, gaussian, sobel_gaussian): ";
	std::cin >> usr_filter;

 	// create grey image
 	Mat grey_image;
 	cvtColor( image, grey_image, CV_BGR2GRAY );
	string grey_file = "out/" + usr_img + "_grey.jpg";
	imwrite(grey_file, grey_image);

	// apply given filter
    if (usr_filter.compare("sobel") == 0) {
        Mat img_sobel_edge_detector;
        int kernel_size = 3;
        SobelEdgeDetector(grey_image, kernel_size, img_sobel_edge_detector);		
        string file_out = "out/" + usr_img + "_sobel_edges.jpg";
		imwrite(file_out, img_sobel_edge_detector);
	}

    else if (usr_filter.compare("sobel_gaussian") == 0) {
        Mat img_gaussian_blurred;
        Mat img_sobel_edge_detector;
        int usr_kernel_size = 3;
        std::cout << "Kernel size: ";
		std::cin >> usr_kernel_size;
        GaussianBlur(grey_image, usr_kernel_size, img_gaussian_blurred);
        int kernel_size = 3;
        SobelEdgeDetector(img_gaussian_blurred, kernel_size, img_sobel_edge_detector);		
		
        string file_out = "out/" + usr_img + "_sobel_gaussian_edges.jpg";
		imwrite(file_out, img_sobel_edge_detector);
	}

	else if (usr_filter.compare("gaussian") == 0) {
		int usr_kernel_size = 3;
        std::cout << "Kernel size: ";
		std::cin >> usr_kernel_size;
		Mat img_gaussian_blurred;
		GaussianBlur(grey_image, usr_kernel_size, img_gaussian_blurred);
		string file_out = "out/" + usr_img + "_gaussian_blurred.jpg";
		imwrite(file_out, img_gaussian_blurred);
	}
	else { 
		std::cout << "Invalid filter" << std::endl; 
		return 0;
	}

	std::cout << "...\n\nFiltering complete!\n" << std::endl;

 	return 0;
}

void SobelEdgeDetector(cv::Mat &input, int size, cv::Mat &sobel_output) {
	
	// intialise the output using the input
	sobel_output.create(input.size(), input.type());

	// df/fx kernel
	double sobel_x_vals[3][3] = { 
		{-1, 0, 1},
		{-1, 0, 1},
		{-1, 0, 1}
	};
    // df/dy kernel
    double sobel_y_vals[3][3] = { 
		{-1,-1,-1},
		{ 0, 0, 0},
		{ 1, 1, 1}
	};

	// create opencv matrix from values
	cv::Mat sobel_x = cv::Mat(size, size, CV_64F, sobel_x_vals);
    cv::Mat sobel_y = cv::Mat(size, size, CV_64F, sobel_y_vals);

	// set kernel radius for convolution
	const int kernel_r_x = (size-1)/2;
	const int kernel_r_y = (size-1)/2;

	// replicate edge at border to allow edge convolutions
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   kernel_r_x, kernel_r_x, kernel_r_y,kernel_r_y,
					   cv::BORDER_REPLICATE);

	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {

			// sum for matrix multiplication
			double sobel_x_sum = 0.0;
            double sobel_y_sum = 0.0;

			// for each kernel value
			for(int m=-kernel_r_x; m<=kernel_r_x; m++) {
				for(int n=-kernel_r_y; n<=kernel_r_y; n++ ) {
					
					// correct image and kernel indices
					int image_x = i + m + kernel_r_x;
					int image_y = j + n + kernel_r_y;
					int kernel_x = m + kernel_r_x;
					int kernel_y = n + kernel_r_y;

					// get the image pixel and kernel value
					int pixel_val = (int)padded_input.at<uchar>(image_x, image_y);
					double sobel_x_val = sobel_x.at<double>(kernel_x, kernel_y);
                    double sobel_y_val = sobel_y.at<double>(kernel_x, kernel_y);
					
					// add their product to current sum
					sobel_x_sum += pixel_val * sobel_x_val;
                    sobel_y_sum += pixel_val * sobel_y_val;
				}
			}

            // grad(f) = ∇|f(x,y)| = sqrt( (df/dx)^2 + (df/dy)^2 )
            double new_pixel = sqrt((sobel_x_sum*sobel_x_sum) + (sobel_y_sum+sobel_y_sum));

			// update the image with new pixel value
			sobel_output.at<uchar>(i, j) = (uchar) new_pixel;
		}
	}
}

void GaussianBlur(cv::Mat &input, int size, cv::Mat &blurred_output) {
	
	// intialise the output using the input
	blurred_output.create(input.size(), input.type());

	// create the Gaussian kernel in 1D 
	cv::Mat kX = cv::getGaussianKernel(size, -1);
	cv::Mat kY = cv::getGaussianKernel(size, -1);

	// make it 2D multiply one by the transpose of the other
	cv::Mat kernel = kX * kY.t();

	// get radius of kernel for border padding
	const int kernel_r_x = (size-1)/2;
	const int kernel_r_y = (size-1)/2;

	// set kernel values
	for( int m=-kernel_r_x; m <=kernel_r_x; m++) {
	  	for( int n=-kernel_r_y; n<=kernel_r_y; n++) {
		   	kernel.at<double>(m+kernel_r_x, n+kernel_r_y) = (double) 1.0/(size*size);
		}
	}

	// pad border of image with duplicate pixels for convolution
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   kernel_r_x, kernel_r_x, kernel_r_y,kernel_r_y,
					   cv::BORDER_REPLICATE);

	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {

			// sum for matrix multiplication
			double pixel_sum = 0.0;

			// for each kernel value
			for(int m=-kernel_r_x; m<=kernel_r_x; m++) {
				for(int n=-kernel_r_y; n<=kernel_r_y; n++) {
					
					// correct image and kernel indices
					int image_x = i + m + kernel_r_x;
					int image_y = j + n + kernel_r_y;
					int kernel_x = m + kernel_r_x;
					int kernel_y = n + kernel_r_y;

					// get the image pixel and kernel value
					int pixel_val = (int)padded_input.at<uchar>(image_x, image_y);
					double kernel_val = kernel.at<double>(kernel_x, kernel_y);
					
					// add their product to current sum
					pixel_sum += pixel_val * kernel_val;
				}
			}

			// update the image with new pixel value
			blurred_output.at<uchar>(i, j) = (uchar) pixel_sum;
		}
	}
}