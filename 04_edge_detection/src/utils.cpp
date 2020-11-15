#include <include/utils.h>

// function to convolve a point(i,j) with a given kernel
double Convolution(cv::Mat &input, cv::Mat &kernel, 
                   int i, int j, const int r_x, const int r_y) {

    // sum for matrix multiplication
    double result = 0.0;
    // for each kernel value
    for(int m=-r_x; m<=r_x; m++) {
		for(int n=-r_y; n<=r_y; n++ ) { 
            // correct image and kernel indices
            int input_i  = i + m + r_x;
            int input_j  = j + n + r_y;
            int kernel_i = m + r_x;
            int kernel_j = n + r_y;
            // get input and kernel values
            double input_val  = input.at<double>(input_i, input_j);
			double kernel_val = kernel.at<double>(kernel_i, kernel_j);
            // add product to current sum
            result += input_val * kernel_val;
        }
    }
    // return convolved input at (i,j)
    return result;

}

// function to threshold values below a given point
void Threshold(const cv::Mat &input, const double threshold_val, 
			   cv::Mat &thresholded_output) {
	
	// intialise the output using the input
	thresholded_output.create(input.size(), input.type());
	
	// for each pixel in input
	for (int i=0; i<input.rows; i++) {	
		for(int j=0; j<input.cols; j++) {
			// get pixel and set output pixel to 0
			double pixel = input.at<double>(i, j);
			double thresholded_pixel = 0;
			// if pixel exceeds threshold set to 255
			if (pixel > threshold_val) {
				thresholded_pixel = pixel;
			}
			// set value in output image
			thresholded_output.at<double>(i, j) = thresholded_pixel;
		}
	}

	std::cout << "\nGradient magnitude thresholding complete!" << std::endl;
}

// function to find maximum in specfied area of vector<Mat> space
// area is (r,x,y) to (r+local_size,x+local_size,y+local_size)
pos FindLocalMaxima(std::vector<cv::Mat> &space,
                    int r, int x, int y, 
	                int local_size) {

	// get max edge of space
	int d_size = space.size();
	int i_size = space[0].rows;
	int j_size = space[0].cols;
	// current and max position and max val
	pos cur_pos = {0, 0, 0};
	pos max_pos = {0, 0, 0};
	double max = 0;

	// for each radius in space
	for (int d=r-local_size; d<r+local_size; d++) {
		// get radius in space and fit
		cur_pos.r = FitToSpace(d, d_size);
		// for each row in space
		for (int i=x-local_size; i<x+local_size; i++) {
			// get row in space and fit
			cur_pos.x= FitToSpace(i, i_size);
			// for each col in space
			for (int j=y-local_size; j<y+local_size; i++) {
				// get col in space and fit
				cur_pos.y = FitToSpace(j, j_size);
				// get value at position in space
				double val_at_pos = space[cur_pos.r].at<double>(cur_pos.x, cur_pos.y);
				// if value is max so far update max and max pos
				if (val_at_pos > max) {
					max = val_at_pos;
					max_pos.r = cur_pos.r;
					max_pos.x = cur_pos.x;
					max_pos.y = cur_pos.y;
				}
			}
		}
	}

	return max_pos;
}


// function to normalise pixels to 0-255
void NormalisePixels(cv::Mat &input, cv::Mat &output) {

	// intialise the output using the input
	output.create(input.size(), cv::DataType<double>::type);

	// get min and max values in input
	double minVal, maxVal; 
	Point minLoc, maxLoc;
	minMaxLoc(input, &minVal, &maxVal, &minLoc, &maxLoc);

	// for each pixel in input
	for (int i=0; i<input.rows; i++) {	
		for (int j=0; j<input.cols; j++) {
			// get current pixel
			double pixel = input.at<double>(i, j);
			// normalise pixel between range 0-255
			double normalised_pixel = 255*((pixel-minVal) / (maxVal-minVal));
			output.at<double>(i, j) = normalised_pixel;
		}
	}

}

// function to write normalised image to file
void NormaliseWrite(string img_name, string out_type, int arg, cv::Mat output) {

	// normalised output image
	cv::Mat normalised_output;
	NormalisePixels(output, normalised_output);

	// create file name based of image input and output type
	string file_name = "out/" + img_name + "_" + out_type;
	file_name += (arg==0 ? ".jpg" : "_" + std::to_string(arg) + ".jpg");

	// write normalised image
	cv::imwrite(file_name, normalised_output);

} 

// function to convert matrix of radians to degrees
void RadToDeg(cv::Mat &input, cv::Mat &output) {

	// intialise the output using the input
	output.create(input.size(), cv::DataType<double>::type);

	// convert each radian to degrees
	for (int i=0; i<input.rows; i++) {	
		for (int j=0; j<input.cols; j++) {
			double rad = input.at<double>(i, j);
			output.at<double>(i,j) = (rad >= 0 ? rad : (2*CV_PI + rad)) * 360 / (2*CV_PI); 
		}
	}

}

// function to fit numbers to range 0-max
int FitToSpace(int x, int max) {

	if      (x<0) return 0;
	else if (x>max) return 1-max;
	return x;
	
}