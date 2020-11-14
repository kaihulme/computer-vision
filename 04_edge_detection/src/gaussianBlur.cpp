#include <include/gaussianBlur.h>

void GaussianBlur(const cv::Mat &input, const int size, 
				  cv::Mat &gaussian_output) {
	
	// intialise the output using the input
	gaussian_output.create(input.size(), input.type());
	
	// create the gaussian kernel
	cv::Mat kX = cv::getGaussianKernel(size, -1);
	cv::Mat kY = cv::getGaussianKernel(size, -1);
	cv::Mat gaussian_kernel = kX * kY.t();
	
	// get radius of kernel for padding and convolution
	const int r_x = (size-1)/2;
	const int r_y = (size-1)/2;
	
	// scale kernel values for size of gaussian kernel
	for( int m=-r_x; m <=r_x; m++) {
	  	for( int n=-r_y; n<=r_y; n++) {
		   	gaussian_kernel.at<double>(m+r_x, n+r_y) = (double) 1.0/(size*size);
		}
	}
	
	// replicate edge at border to allow edge convolutions
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   r_x, r_x, r_y,r_y,
					   cv::BORDER_REPLICATE);
	
	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {
			// sum for matrix multiplication
			double gaussian_sum = 0.0;
			// for each kernel value
			for(int m=-r_x; m<=r_x; m++) {
				for(int n=-r_y; n<=r_y; n++) {
					// correct image and kernel indices
					int img_i    = i + m + r_x;
					int img_j    = j + n + r_y;
					int kernel_i = m + r_x;
					int kernel_j = n + r_y;
					// get the image pixel value and kernel value
					int img_val                = (int)padded_input.at<uchar>(img_i, img_j);
					double gaussian_kernel_val = gaussian_kernel.at<double>(kernel_i, kernel_j);
					// add their product to current sum
					gaussian_sum += img_val * gaussian_kernel_val;
				}
			}
			// update the image with new pixel value
			gaussian_output.at<uchar>(i, j) = (uchar) gaussian_sum;
		}
	}

	std::cout << "\nGaussian blur complete!" << std::endl;
}