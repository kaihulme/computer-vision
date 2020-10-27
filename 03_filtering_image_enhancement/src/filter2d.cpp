// header inclusion
#include <stdio.h>
#include <string>
#include <iostream>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

using namespace cv;

void GaussianBlur(cv::Mat &input, 
				  int size,
	cv::Mat &blurredOutput);

void Sharpen(cv::Mat &input, 
			 int size,
	         cv::Mat &sharpenedOutput);

void UnsharpMask(cv::Mat &input, 
				 int size,
				 cv::Mat &sharpenedOutput);

void MedianFilter(cv::Mat &input, 
			int size, 
			cv::Mat &medianOutput);

void quickSort(int array[], int low, int high);
int get_pivot(int array[], int low, int high);
void swap(int* a, int* b);

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
	std::cout << "Filter type (blur, sharpen, unsharpen, median): ";
	std::cin >> usr_filter;

 	// create grey image
 	Mat gray_image;
 	cvtColor( image, gray_image, CV_BGR2GRAY );
	string grey_file = "out/" + usr_img + "_grey.jpg";
	imwrite(grey_file, gray_image);

	// get kernel size (if not sharpen)
	int usr_kernel_size = 5;
	if (usr_filter.compare("sharpen") != 0) {
		std::cout << "Kernel size: ";
		std::cin >> usr_kernel_size;
	}

	// apply given filter
	if (usr_filter.compare("blur") == 0) {
		Mat carBlurred;
		GaussianBlur(gray_image, usr_kernel_size, carBlurred);
		string file_out = "out/" + usr_img + "_blurred.jpg";
		imwrite(file_out, carBlurred);
	}
	else if (usr_filter.compare("sharpen") == 0) {
		Mat carSharpened;
		const int sharpen_iterations = 1;
		for(int i=0; i<sharpen_iterations; i++) {
			Sharpen(gray_image, usr_kernel_size, carSharpened);
		}
		string file_out = "out/" + usr_img + "_sharpened.jpg";
		imwrite(file_out, carSharpened);
	}
	else if (usr_filter.compare("unsharpen") == 0) {		
		Mat carUSharpened;
		UnsharpMask(gray_image, usr_kernel_size, carUSharpened);
		string file_out = "out/" + usr_img + "_unsharpened.jpg";
		imwrite(file_out, carUSharpened);
	}
	else if (usr_filter.compare("median") == 0) {		
		Mat carMedianFiltered;
		MedianFilter(gray_image, usr_kernel_size, carMedianFiltered);
		string file_out = "out/" + usr_img + "_median_filtered.jpg";
		imwrite(file_out, carMedianFiltered);
	}
	else { 
		std::cout << "Invalid filter" << std::endl; 
		return 0;
	}

	std::cout << "...\n\nFiltering complete!\n" << std::endl;

 	return 0;
}

void GaussianBlur(cv::Mat &input, int size, cv::Mat &blurredOutput) {
	
	// intialise the output using the input
	blurredOutput.create(input.size(), input.type());

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
			blurredOutput.at<uchar>(i, j) = (uchar) pixel_sum;
		}
	}
}

void Sharpen(cv::Mat &input, int size, cv::Mat &sharpenedOutput) {
	
	// intialise the output using the input
	sharpenedOutput.create(input.size(), input.type());

	// unsharp masking kernel
	double kernel_vals[5][5] = { 
		{1.0,  4.0,    6.0,  4.0, 1.0},
		{4.0, 16.0,   24.0, 16.0, 4.0},
		{6.0, 24.0, -476.0, 24.0, 6.0},
		{4.0, 16.0,   24.0, 16.0, 4.0},
		{1.0,  4.0,    6.0,  4.0, 1.0}
	};

	// normalise kernel
	for (int i=0; i<size; i++) {
		for (int j=0; j<size; j++) {
			kernel_vals[j][i] = kernel_vals[j][i] / -256;
		}
	}

	// create opencv matrix from values
	cv::Mat kernel = cv::Mat(size, size, CV_64F, kernel_vals);

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
			double pixel_sum = 0.0;

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
					double kernel_val = kernel.at<double>(kernel_x, kernel_y);
					
					// add their product to current sum
					pixel_sum += pixel_val * kernel_val;
				}
			}

			// update the image with new pixel value
			sharpenedOutput.at<uchar>(i, j) = (uchar) pixel_sum;
		}
	}
}

void UnsharpMask(cv::Mat &input, int size, cv::Mat &sharpenedOutput) {
	
	// apply gaussian blur to input
	cv::Mat carGaussianBlurred;
	GaussianBlur(input, size, carGaussianBlurred);

	// subtract blurred image from 2*input to apply unsharp masking
	sharpenedOutput = (2 * input) - carGaussianBlurred;
}

void MedianFilter(cv::Mat &input, int size, cv::Mat &medianOutput) {

	// initialise the output using the input
	medianOutput.create(input.size(), input.type());

	// set flattened kernel length and kernel radius
	const int filter_length = size*size;
	const int filter_r_x = (size - 1) / 2;
	const int filter_r_y = (size - 1) / 2;

	// replicate edge at border to allow edge calculations
	cv::Mat paddedInput;
	cv::copyMakeBorder(input, paddedInput, 
					   filter_r_x, filter_r_x, filter_r_y, filter_r_y,
					   cv::BORDER_REPLICATE );

	// for each pixel in input
	for (int i = 0; i < input.rows; i++) {	
		for(int j = 0; j < input.cols; j++) {

			// initiliase flattened filter
			int filter_vals[filter_length];

			// for each filter position
			for(int m=-filter_r_x; m<=filter_r_x; m++) {
				for (int n=-filter_r_y; n<=filter_r_y; n++) {

					// correct image and filter indices
					int image_x = i + m + filter_r_x;
					int image_y = j + n + filter_r_y;
					int filter_i = ((m+filter_r_x)*size) + n + 1;

					// add filter value to flattened filter
					filter_vals[filter_i] = (int)paddedInput.at<uchar>(image_x, image_y);
				}
			}

			// apply quicksort to flattened filter values
			quickSort(filter_vals, 0, filter_length-1);

			// get median of filter values
			int mid_i = (filter_length-1)/2;
			uchar median = (uchar)filter_vals[mid_i];

			// update the image with new pixel value
			medianOutput.at<uchar>(i, j) = median;
		}
	}
}

void quickSort(int array[], int low, int high) {
	// if low pivot is less than high pivot
	if (low < high) {

		// sort pivot and get position
		int pivot_i = get_pivot(array, low, high);

		// sort values before pivot
		quickSort(array, low, pivot_i-1);
		// sort values after pivot
		quickSort(array, pivot_i+1, high);		
	}
}

int get_pivot(int array[], int low, int high) {
	int pivot = array[high];
	int i = (low - 1); 
    for (int j = low; j <= high- 1; j++) { 
	    if (array[j] <= pivot) { 
		    i++; 
		    swap(&array[i], &array[j]); 
        } 
    } 
    swap(&array[i + 1], &array[high]); 
    return (i + 1); 
}

void swap(int* a, int* b) {
	int t = *a; // create temp copy of a 
	*a = *b; // set a as b
	*b = t; // set b as a (temp)
}