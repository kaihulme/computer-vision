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

    // open file
    string file_name = "args_help.txt";
    std::ifstream f(file_name);

    // output file contents
    std::cout<<"\n"<<std::endl;
    if (f.is_open()) std::cout << f.rdbuf();
    std::cout<<"\n"<<std::endl;

}