#include <include/sobelEdges.h>
#include <include/utils.h>

void SobelEdgeDetector(const cv::Mat &input, const int size, 
					   cv::Mat &dfdx_output, cv::Mat &dfdy_output,
					   cv::Mat &magnitude_output, cv::Mat &direction_output) {
	
	// intialise the output using the input
	dfdx_output.create(input.size(), cv::DataType<double>::type);
	dfdy_output.create(input.size(), cv::DataType<double>::type);
	magnitude_output.create(input.size(), cv::DataType<double>::type);
	direction_output.create(input.size(), cv::DataType<double>::type);
	
	// df/fx kernel
	double dfdx_kernel_vals[3][3] = {{-1, 0, 1},
						   			 {-1, 0, 1},
									 {-1, 0, 1}};
    // df/dy kernel
    double dfdy_kernel_vals[3][3] = {{-1,-1,-1},
									 { 0, 0, 0},
									 { 1, 1, 1}};
	
	// create opencv matrix from values
	cv::Mat dfdx_kernel = cv::Mat(size, size, cv::DataType<double>::type, dfdx_kernel_vals);
    cv::Mat dfdy_kernel = cv::Mat(size, size, cv::DataType<double>::type, dfdy_kernel_vals);
	
	// set kernel radius for padding and convolution
	const int r_x = (size-1)/2;
	const int r_y = (size-1)/2;
	
	// replicate edge at border to allow edge convolutions
	cv::Mat padded_input;
	cv::copyMakeBorder(input, padded_input, 
					   r_x, r_x, r_y, r_y,
					   cv::BORDER_REPLICATE);
	
	// apply convolution to each pixel in image
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {
            // apply dfdx and dfdy kernel convolutions to current pixel
			double dfdx_pixel = Convolution(padded_input, dfdx_kernel, i, j, r_x, r_y);
			double dfdy_pixel = Convolution(padded_input, dfdy_kernel, i, j, r_x, r_y);
	        // magnitude: ∇|f(x,y)| = sqrt( (df/dx)^2 + (df/dy)^2 )
			// direction: φ = arctan( (df/dy) / (df/dx) )
            double magnitude_pixel = sqrt((dfdx_pixel*dfdx_pixel) + (dfdy_pixel*dfdy_pixel));
			double direction_pixel = atan2(dfdy_pixel, dfdx_pixel);	
			// update the image with new pixel value
			dfdx_output.at<double>(i, j)      = dfdx_pixel;
			dfdy_output.at<double>(i, j)      = dfdy_pixel;
			magnitude_output.at<double>(i, j) = magnitude_pixel;
			direction_output.at<double>(i, j) = direction_pixel;
		}
	}

	std::cout << "\nSobel edge detection complete!" << std::endl;
}