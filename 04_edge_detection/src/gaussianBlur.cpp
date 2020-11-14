#include <include/gaussianBlur.h>
#include <include/utils.h>

void GaussianBlur(const cv::Mat &input, const int size, 
				  cv::Mat &gaussian_output) {
	
	// intialise the output using the input
	gaussian_output.create(input.size(), cv::DataType<double>::type);
	
	// create the gaussian kernel
	cv::Mat kX = cv::getGaussianKernel(size, -1);
	cv::Mat kY = cv::getGaussianKernel(size, -1);
	cv::Mat gaussian_kernel = kX * kY.t();
	
	// get radius of kernel for padding and convolution
	const int r_x = (size-1)/2;
	const int r_y = (size-1)/2;
	
	// replicate edge at border to allow edge convolutions
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   r_x, r_x, r_y,r_y,
					   cv::BORDER_REPLICATE);
	
	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {
			// apply convolution to current pixel
            double gaussian_pixel = Convolution(padded_input, gaussian_kernel, i, j, r_x, r_y);
			// update the image with new pixel value
			gaussian_output.at<double>(i, j) = gaussian_pixel;
		}
	}

	std::cout << "\nGaussian blur complete!" << std::endl;
}