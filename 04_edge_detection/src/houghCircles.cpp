#include <include/houghCircles.h>

void GetHoughSpaceCircles(const cv::Mat &input, int r_size,
						  int min_r, int max_r, int r_step, int t_step,
						  std::vector<cv::Mat> &hough_space_circles_output) {

	// initalise hough_circles output
	cv::Mat hough_space_circles;
	hough_space_circles.create(input.size(), cv::DataType<double>::type);

	// set current radius to min radius
	int radius = min_r;

	// for each radii
	for (int r=0; r<=r_size; r++) {

		// reset hough space for current radius
		for (int i=0; i<input.rows; i++) {	
			for(int j=0; j<input.cols; j++) {
				hough_space_circles.at<double>(i, j) = 0.0;
			}
		}

		// for each pixel in image
		for (int i=0; i<input.rows; i++) {	
			for(int j=0; j<input.cols; j++) {
				// get gradient magnitude at (i,j)
				double pixel_val = input.at<double>(i,j);
				// if gradient magnitude is not 0
				if (pixel_val > 0) {
					// for circle around point
					for (int theta=0; theta<360; theta+=t_step){
						// get circle coordinates at and angle theta
						double x0 = i - (radius * cos(theta*CV_PI/180));
						double y0 = j - (radius * sin(theta*CV_PI/180));
						// increment value in hough space (unless out of image)
						if (x0>=0 && y0>=0 && x0<input.rows && y0<input.cols) {
							hough_space_circles.at<double>(x0,y0)++;
						}
					}
				}
			}
		}

		// add hough space for circle radius r to vector of spaces
		hough_space_circles_output.push_back(hough_space_circles.clone());
		// std::cout << r+1 << ": computed r=" << radius << std::endl;
		radius += r_step;

	}

	std::cout << "\nHough transform circles complete!" << std::endl;
}

void SumHoughSpaceCircles(std::vector<cv::Mat> &hough_space_circles,
						  cv::Mat &hough_space_circles_summed) {

    // set summed space as 
    hough_space_circles_summed.create(hough_space_circles[0].size(), cv::DataType<double>::type);

    // set hough space sum to 0
    for (int i=0; i<hough_space_circles_summed.rows; i++) {	
        for(int j=0; j<hough_space_circles_summed.cols; j++) {
            hough_space_circles_summed.at<double>(i, j) = 0.0;
        }
    }

    // for each hough space add value to summed hough space
    for (auto space : hough_space_circles) {
        for (int i=0; i<hough_space_circles_summed.rows; i++) {
            for (int j=0; j<hough_space_circles_summed.cols; j++) {
                hough_space_circles_summed.at<double>(i,j) += space.at<double>(i,j);
            }
        }
    }

}

void findHoughCircles(std::vector<cv::Mat> &hough_space_circles, double threshold,
					  std::vector<pos> &hough_circle_locs) {

	int d_size = hough_space_circles.size();
	int i_size = hough_space_circles[0].rows;
	int j_size = hough_space_circles[0].cols;

	// threshold each hough space
	for (int d=0; d<d_size; d++) {
		cv::Mat thresholded_space;
		Threshold(hough_space_circles[d], threshold, thresholded_space);
		hough_space_circles[d] = thresholded_space;
	}

	// local maxima search size
	int local_size = 10;

	// for each in hough space
	for (int d=0; d<d_size; d++) {
		for (int i=0; i<i_size; i++) {
			for (int j=0; j<j_size; j++) {
				// get local maxima at point
				pos loc = FindLocalMaxima(hough_space_circles, d, i, j, local_size);
				// add circle position if there is a local maxima
				if (loc.r+loc.x+loc.y > 0) hough_circle_locs.push_back(loc);
			}
		}
	}

}


