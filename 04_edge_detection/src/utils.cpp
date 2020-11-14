#include <include/utils.h>

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
				thresholded_pixel = 255;
			}
			// set value in output image
			thresholded_output.at<double>(i, j) = thresholded_pixel;
		}
	}

	std::cout << "\nGradient magnitude thresholding complete!" << std::endl;
}

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

void NormaliseWrite(string img_name, string out_type, int arg, cv::Mat output) {

	// normalised output image
	cv::Mat normalised_output;
	NormalisePixels(output, normalised_output);

	// create file name based of image input and output type
	string file_name = "out/" + img_name + "_" + out_type;
	file_name += (arg==0 ? ".jpg" : "_" + std::to_string(arg) + ".jpg");



	// if (arg==0) file_name += ".jpg";
	// else 			file_name += "_" + std::to_string(arg) + ".jpg";




	// write normalised image
	cv::imwrite(file_name, normalised_output);

} 

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

void ArgsHelper() {

	std::cout << "\nEDGES COMMAND LINE HELPER" << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------------------" << std::endl;
	std::cout << "\n- the first argument should be the file to be transformed\n" << std::endl;
	std::cout << "	- ignore image file extension (.png, .jpg etc.)" << std::endl;
	std::cout << "	- image should be places in /resources" << std::endl;
	std::cout << "\n- (-g [x]) will apply a gaussian blur:\n" << std::endl;
	std::cout << "	- x specifies the kernel size" << std::endl;
	std::cout << "	- not specifying x will deafult to kernel size 3" << std::endl;
	std::cout << "	- applying a blur will remove some noise and improve performance of the edge detector" << std::endl;
	std::cout << "	- this will output [file]_gaussian to /out" << std::endl;
	std::cout << "\n- (-s) will apply the sobel edge detection:\n" << std::endl;
	std::cout << "	- this will output the following to /out" << std::endl;
	std::cout << "		- [file]_dfdx (horizontal gradients)" << std::endl;
	std::cout << "		- [file]_dfdy (vertical gradients)" << std::endl;
	std::cout << "		- [file]_magnitude (magnitude of gradients: ∇|f(x,y)| = sqrt( (df/dx)^2 + (df/dy)^2 ) )" << std::endl;
	std::cout << "		- [file]_direction (angle of gradients: φ = arctan( (df/dy) / (df/dx) ) )" << std::endl;
	std::cout << "\n- (-h [min][max][r_step][t_step]) will apply hough transform circles:\n" << std::endl;
	std::cout << "	- min specifies the minimum radius for the hough transform circles" << std::endl;
	std::cout << "		- not specifying min will default to 10" << std::endl;
	std::cout << "		- this size may not work on your image so chance acoordingly" << std::endl;
	std::cout << "	- max specifies the maximum radius for the hough transform circles" << std::endl;
	std::cout << "		- not specifying max will default to 100" << std::endl;
	std::cout << "		- this size may not work on your image so chance acoordingly" << std::endl;
	std::cout << "	- r_step specifies the step between each hough transform radii" << std::endl;
	std::cout << "		- not specifying r_step will default to 1" << std::endl;
	std::cout << "		- increasing should improve processing time at the cost of accuracy" << std::endl;
	std::cout << "	- t_step specifies the step between the angles the hough transform circles are drawn" << std::endl;
	std::cout << "		- not specifying t_step will default to 1" << std::endl;
	std::cout << "		- increasing should improve processing time at the cost of accuracy" << std::endl;
	std::cout << "\n- (-t [x]) will threshold the values of the gradient magnitudes:\n" << std::endl;
	std::cout << "	- this will output [file]_thresholded to /out" << std::endl;
	std::cout << "	- x is the value to threshold at: lower will be set to black, equal to or higher to white" << std::endl;
	std::cout << "		- not specifying x will default to 100" << std::endl;
	std::cout << "	- check the output image and adjust x to highlight edges for improved hough transform\n\n" << std::endl;

}