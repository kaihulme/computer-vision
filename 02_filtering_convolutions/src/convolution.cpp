#include <stdio.h>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

using namespace cv;

int main() {

	// read image in as opencv matrix
	Mat image;
	image = imread("resources/mandrill.jpg", CV_LOAD_IMAGE_UNCHANGED);

	int kernel_size;
	std::cout << "\nPlease enter kernel size: ";
  	std::cin >> kernel_size;
	std::cout << "..." << std::endl;

	// n*n low pass filter
	double k_v = (double)1/(double)(kernel_size*kernel_size);
	double kernel_vals[kernel_size][kernel_size];
	for (int i=0; i<kernel_size; i++) {
		for (int j=0; j<kernel_size; j++) {
			kernel_vals[j][i] = k_v;
		}
	}

	// create opencv matrix from values
	cv::Mat kernel = cv::Mat(kernel_size, kernel_size, CV_64F, kernel_vals);

	// set kernel radius for convolution
	const int kernel_r_x = (kernel_size-1) / 2;
	const int kernel_r_y = (kernel_size-1) / 2;

	// replicate edge at border to allow edge convolutions
	cv::Mat padded_image;
	cv::copyMakeBorder( image, padded_image, 
						kernel_r_x, kernel_r_x, kernel_r_y,kernel_r_y,
						cv::BORDER_REPLICATE );
					
	// for each pixel in the image
	for (int i=0; i<image.rows; i++) {	
		for(int j=0; j<image.cols; j++) {

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
					int pixel_val = (int)padded_image.at<uchar>(image_x, image_y);
					double kernel_val = kernel.at<double>(kernel_x, kernel_y);
					
					// add their product to current sum
					pixel_sum += pixel_val * kernel_val;
				}
			}

			// update the image with new pixel value
			image.at<uchar>(i,j) = (uchar) pixel_sum;
		}
	}

	// write the updated image to file
	imwrite("out/convolution.jpg", image);

	std::cout << "\nConvolution complete!\n" << std::endl;

	return 0;
}