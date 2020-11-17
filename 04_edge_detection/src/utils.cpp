#include <include/utils.h>

// function to convolve a point(i,j) with a given kernel
double Convolution(cv::Mat &input, cv::Mat &kernel, 
                   int x, int y, const int r_x, const int r_y) {

    // sum for matrix multiplication
    double result = 0.0;
    // for each kernel value
    for(int m=-r_x; m<=r_x; m++) {
		for(int n=-r_y; n<=r_y; n++ ) { 
            // correct image and kernel indices
            int input_x  = x + m + r_x;
            int input_y  = y + n + r_y;
            int kernel_x = m + r_x;
            int kernel_y = n + r_y;
            // get input and kernel values
            double input_val  = input.at<double>(input_y, input_x);
			double kernel_val = kernel.at<double>(kernel_y, kernel_x);
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
	for (int y=0; y<input.rows; y++) {	
		for(int x=0; x<input.cols; x++) {

			// get pixel and set output pixel to 0
			double pixel = input.at<double>(y, x);
			double thresholded_pixel = 0;

			// if pixel exceeds threshold set to 255
			if (pixel > threshold_val) {
				thresholded_pixel = pixel;
			}

			// set value in output image
			thresholded_output.at<double>(y, x) = thresholded_pixel;
		}
	}

}

// // function to find maximum in specfied area of vector<Mat> space
// // area is (r,x,y) to (r+local_size,x+local_size,y+local_size)
// pos_t FindLocalMaxima(std::vector<cv::Mat> &hough_space_circles,
//                     int r, int x, int y, 
// 	                int local_size) {

// 	// get max edge of space
// 	int d_size = hough_space_circles.size();
// 	int i_size = hough_space_circles[0].rows;
// 	int j_size = hough_space_circles[0].cols;
// 	// current and max position and max val
// 	pos_t cur_pos = {0, 0, 0};
// 	pos_t max_pos = {0, 0, 0};
// 	double max = 0;

// 	cv::Mat temp_space = hough_space_circles[0];

// 	int local_radius_size = 2;

// 	// for each radius in space
// 	for (int d=r-local_radius_size; d<r+local_radius_size; d+=local_radius_size) {

// 		// get radius in space and fit
// 		cur_pos.r = FitToSpace(d, d_size);

// 		if      (cur_pos.r < 0)      d = 0;
// 		else if (cur_pos.r > d_size) continue;

// 		// for each row in space
// 		for (int i=x-local_size; i<x+local_size; i+=local_size) {

// 			// get row in space and fit
// 			cur_pos.x= FitToSpace(i, i_size);

// 			if      (cur_pos.x < 0)      i = 0;
// 			else if (cur_pos.x > i_size) continue;

// 			// for each col in space
// 			for (int j=y-local_size; j<y+local_size; j+=local_size) {

// 				// get col in space and fit
// 				cur_pos.y = FitToSpace(j, j_size);

// 				if      (cur_pos.y < 0)      j = 0;
// 				else if (cur_pos.y > j_size) continue;

// 				// get value at position in space
// 				double val_at_pos = temp_space.at<double>(cur_pos.x, cur_pos.y);

// 				// if value is max so far update max and max pos
// 				if (val_at_pos > max) {
					
// 					max = val_at_pos;
// 					max_pos.r = cur_pos.r;
// 					max_pos.x = cur_pos.x;
// 					max_pos.y = cur_pos.y;

// 					std::cout << "max in region (" << cur_pos.r << ", " << cur_pos.x << ", " << cur_pos.y 
// 							  << ")	=	" << max_pos.r << ", " << max_pos.x << ", " << max_pos.y << std::endl;

// 				}

// 				temp_space.at<double>(cur_pos.x, cur_pos.y) = 0;

// 			}
// 		}
// 	}

// 	return max_pos;
// }


// finds a max in region (x,y)->(x+r,y+r) with border
// removes region once max is found to remove duplicates
pos_t LocalMax(cv::Mat &space, int i, int j, int border,
			   int region_size) {

	int x_max = space.cols;
	int y_max = space.rows;

	double max 	   = 0;
	pos_t  max_pos = {0,0};

	for (int y=j-region_size; y<j+region_size; y++) {	
		for(int x=i-region_size; x<i+region_size; x++) {

			if (x>=0 && x<x_max && y>=0 && y<y_max) {

				// get val at (i,j)
				double val = space.at<double>(y, x);

				// std::cout << "(" << x << ", " << y << ") : " << val << std::endl;

				if (val > max) { 

					// std::cout << "\ni : " << i << ", j : " << j << std::endl;
					// std::cout << "max: " << val << " > " << max << std::endl;

					// if highest set new max
					max     = val; 
					max_pos = {x,y};
					
				}
				// else std::cout << "not max: " << val << " <= " << max << std::endl;
			}

		}
	}

	// if (max > 0) {
	// 	std::cout << "\nMAX at : " << "(" << max_pos.x << ", " 
	// 	   		  << max_pos.y << ") = " << max << std::endl;
	// }

	for (int y=j-region_size; y<j+region_size; y++) {	
		for(int x=i-region_size; x<i+region_size; x++) {
			if (x>=0 && x<x_max && y>=0 && y<y_max) {
				// remove region from space
				space.at<double>(y, x) = 0;
			}
		}
	}

	return max_pos;

}

// function to draw circles at given positions on image
void DrawCircles(cv::Mat &image, std::vector<circle_t> &circles, 
				 std::string image_name) {

	cv::Mat img_circle = image;

	//create a red 256x256, 8bit, 3channel BGR image in a matrix container
	cv::Mat out_image(image.size(), CV_8UC3, Scalar(0, 0, 255));

	for (auto circle : circles) {

		// correct radius of circle
		int radius  = circle.radius;
		// get circle centre position
		cv::Point point = cv::Point(circle.pos.x, circle.pos.y);

		// std::cout << "drawing circle - radius : " << radius 
		//           << ", centre : (" << circle.pos.x 
		//           << ", " << circle.pos.y << ")" << std::endl;
		
		// draw circle
		cv::circle(image, point, radius, cvScalar(124, 200, 73), 2);
		// mark centre
		cv::circle(image, point, 4, cvScalar(50, 50, 240), -1);

	}

	// NormaliseWrite(image_name, "CIRCLES", 0, out_image);
	// imwrite("out/COINSWITHCIRCLS.jpg", out_image);
	imwrite("out/COINSWITHCIRCLS.jpg", image);

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
	for (int y=0; y<input.rows; y++) {	
		for(int x=0; x<input.cols; x++) {
			// get current pixel
			double pixel = input.at<double>(y, x);
			// normalise pixel between range 0-255
			double normalised_pixel = 255*((pixel-minVal) / (maxVal-minVal));
			output.at<double>(y, x) = normalised_pixel;
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
	for (int y=0; y<input.rows; y++) {	
		for(int x=0; x<input.cols; x++) {
			double rad = input.at<double>(y, x);
			output.at<double>(y, x) = (rad >= 0 ? rad : (2*CV_PI + rad)) * 360 / (2*CV_PI); 
		}
	}

}

// function to fit numbers to range 0-max
int FitToSpace(int x, int max) {

	if      (x<0)    return 0;
	else if (x>=max) return max-1;
	return x;

}