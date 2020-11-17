#include <include/circleDetector.h>

// find circles in img
std::vector<circle_t> FindCircles(std::vector<cv::Mat> &input,
								  cv::Mat &hough_space_sum,
								  int r_size, int min_r, int r_step) {
	
	// create vector of circles
	std::vector<circle_t> circles_output;

	std::cout << "\n--------\n\nStarting circle detection" << std::endl;

	// get circle centres from summed hough space 
	std::vector<pos_t> circle_locs = GetCircleLocs(hough_space_sum);

	int c = 0;
	int c_size = circle_locs.size();

	std::cout << "\n-----------\n" << std::endl;
	std::cout << "c_size : " << c_size << ", r_size : " 
			  << r_size << std::endl;

	for (pos_t &circle_loc : circle_locs) {

		std::cout << "\n-----------\n" << std::endl;
		std::cout << "Finding radius for circle "<< c << ", at: (" 
				  << circle_loc.x << ", " << circle_loc.y << ")\n" 
				  << std::endl;

		// best radius and position counter
		double max_r = 0.0;
		int    r_pos = 0;
		
		// std::cout << "\n\nhough_size_in_cd " << input.size() << "\n\n" << std::endl;

		// find best radius
		int r = 0;
		// for (int r=0; r<r_size; r++) {
		for (cv::Mat &space : input) {

			std::cout << "radius: " << min_r+r_step*r_pos << ", weight = " 
			          << space.at<double>(circle_loc.y, circle_loc.x) 
					  << std::endl;

			// get current radius
			double curr_r = space.at<double>(circle_loc.y, circle_loc.x);

			// if best radius
			if (curr_r > max_r) {
				max_r = curr_r;
				r_pos = r;
			}

			r++;

		}

		
		// set radius
		int circle_radius = min_r + r_step*r_pos;

		std::cout << "\nCircle " << c << ": radius = " << circle_radius << std::endl;
		
		// create circle
		circle_t circle = {circle_locs[c], circle_radius};
		
		// add circle to vector
		circles_output.push_back(circle);
		
		c++;

	}

	return circles_output;
}
	
// return circle locations as local maxima in img
std::vector<pos_t> GetCircleLocs(cv::Mat input) {

	// create vector of circle locations
	std::vector<pos_t> circle_locs;
	
	// create vector of weighted locations
	std::vector<w_pos_t> weighted_locs;

	int c = 0;

	// for each pixel
	for (int y=0; y<input.rows; y++) {	
		for(int x=0; x<input.cols; x++) {

			// std::cout << "checking neighbours of (" << i << ", " 
			//             << j << ")" << std::endl;
			
			// get current value
			double curr = input.at<double>(y, x);
			
			// ignore blank
			if (curr > 0) {
			
				// get local max
				// pos_t pos		 = {i, j};

				// find local max then remove region
				int border = 5;
				int region_size = 10;
				pos_t circle_pos = LocalMax(input, x, y, border, region_size);

				// pos_t circle_pos = GetBestNeighbour(img, pos, curr);
				
				// get weighted position of circle
				double weight = input.at<double>(y, x);
				w_pos_t weighted_loc = {circle_pos, weight};

				// add weighed location to vector
				weighted_locs.push_back(weighted_loc);

				std::cout << "\nfor (" << x << ", " << y << ") :" 
				          << "found circle centre at (" << circle_pos.x  
						<< ", " << circle_pos.y << ")" << std::endl;

				c++;
			
			}

			std::cout << "\npixel: " << x << ", " << y << std::endl;
			
		}
	}

	std::cout << "\nFound " << c << " circles" << std::endl;
	
	// int dist = 20;
	// remove any neighbours which are closer than dist
	// RemoveCloseNeighbours(weighted_locs, dist);
	// std::cout << "Removed close circles\n" << std::endl;
	
	// add circle positions to vector
	for (auto loc : weighted_locs) circle_locs.push_back(loc.pos);
	
	return circle_locs;
}

// // remove any circles closer than dist
// void RemoveCloseNeighbours(std::vector<w_pos_t> &circle_locs, int dist) {

// 	// vector to remove from
// 	std::vector<w_pos_t> sparse_locs = circle_locs;
		
// 	// compare each circles distance
// 	for (w_pos_t circle_i : circle_locs) {
// 		for (w_pos_t circle_j : circle_locs) {
// 			// if not comparing to self and distance is less than dist
// 			if (!PosEqual(circle_i.pos, circle_j.pos) ) {
// 				if (CalcDist(circle_i.pos, circle_j.pos) < dist) {
// 					// remove circle with least weight
// 					if (circle_i.weight < circle_j.weight)
// 						RemoveCircle(circle_i, sparse_locs); 
// 					else RemoveCircle(circle_j, sparse_locs); 
// 				}
// 			}
// 		}
// 	}
	
// 	circle_locs = sparse_locs;
// }

// // removes a circle position of given value
// void RemoveCircle(w_pos_t circle, std::vector<w_pos_t> &sparse_locs) {
	
// 	// get number of circles
// 	int num_locs = sparse_locs.size();

// 	// for each circle
// 	for (int i=0; i<num_locs; i++) {
// 		// if i is target remove and return
// 		if (PosEqual(circle.pos, sparse_locs[i].pos)) {
// 			sparse_locs.erase(sparse_locs.begin()+i);
// 			return;
// 		}
// 	}

// }
			
// // recursively heads towards the best neighbour
// pos_t GetBestNeighbour(cv::Mat img, pos_t pos, double curr) {

// 	// next position to check
// 	pos_t next_pos = pos;

// 	// find best of 9 surrounding neighbours
// 	for (int i=-3; i<=3; i++) {
// 		for (int j=-3; j<=3; j++) {		
		
// 			// get neighbour
// 			double neighbour = img.at<double>(i,j);
			
// 			// set curr as next if higher
// 			if (neighbour > curr) {
// 				curr     = neighbour;
// 				next_pos = {i, j};
// 			}
			
// 		}
// 	}
	
// 	// if next position is current return current
// 	if (PosEqual(pos, next_pos)) return pos;
// 	// else get best neighbour of next
// 	return GetBestNeighbour(img, next_pos, curr);
// }

// // calculate distance between two points
// double CalcDist(pos_t a, pos_t b) {
// 	// get x and y distances
// 	double dx = a.x - b.x;
// 	double dy = a.y - b.y;
// 	// return euclidean distance
// 	return sqrt( dx*dx + dy*dy );
// }

// // checks two weighed positions are not equal
// bool PosEqual(pos_t a, pos_t b) {
// 	if (a.x==b.x && a.y==b.y) return true;
// 	return false;
// }