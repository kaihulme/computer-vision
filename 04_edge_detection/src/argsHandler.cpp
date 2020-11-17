#include <include/argsHandler.h>

// handles command line arguments
int ArgsHandler(int     argc,               // no. of arguments
                char    *argv[],            // arguments
                cv::Mat &image,             // image
                string  &image_name,        // image name
                bool    &sobel,             // whether or not to apply sobel edge detection
                bool    &hough_circles,     // whether or not to apply hough transform (circles)
                bool    &threshold,         // whether or not to apply magnitude thresholding
                bool    &gaussian,          // whether or not to apply gaussian smoothing
	            int     &gaussian_val,      // gaussian kernel size
                int     &threshold_val,     // threshold for sobel gradient magnitude
	            int     &min_r,             // min radius for hough circles
                int     &max_r,             // max radius for hough circles
	            int     &r_step,            // radius stepping for hough circles
                int     &t_step,            // theta stepping for hough circles
                int     &threshold_h){      // hough space threshold

    // set default values
    sobel = false; hough_circles = false;
    threshold = false; gaussian = false;
	gaussian_val = 3; threshold_val = 100;
	min_r = 5; max_r = 50; 
    r_step = 1; t_step = 1;
	threshold_h = 10;

    // check arg count
	if (argc < 2)  { 
        printf("\nError: image not specified!\n\n");
        return -1; 
    }
	
    // if ? show args help
	if (!strcmp(argv[1], "?")) {
        ArgsHelper(); 
        return -1;
    }

    // if only image argument specified
	else if (argc == 2) {
        printf("\nError: operation not specified!\n\n"); 
        return -1;
    }

	// get image location
	image_name = argv[1];
	const string img_loc = "resources/" + image_name + ".png";
	
	// read image data
 	const Mat image_read = imread(img_loc, 1);
 	if(!image_read.data ) {
   		printf("\nError: image not found!\n\n");
   		return -1;
 	} 

	// convert to grey double Matrix
 	cv::Mat img_grey;
 	cvtColor(image_read, img_grey, CV_BGR2GRAY);
	img_grey.convertTo(image, cv::DataType<double>::type);
	
	// handle flags and set bools
	for (int i=2; i<argc; i++) {

		// if -s apply sobel edge detection
		if (!strcmp(argv[i], "-s") && !sobel) sobel = true;

		// if -h [x][y][z] apply hough transform circles with radii x->y step z
		else if (!strcmp(argv[i], "-h") && !hough_circles) { 
			try {
				min_r  		= std::stoi(argv[i+1]); i++;
				max_r  		= std::stoi(argv[i+1]); i++;
				r_step 		= std::stoi(argv[i+1]); i++;
				t_step 		= std::stoi(argv[i+1]); i++;
				threshold_h = std::stoi(argv[i+1]); i++;
			}
			catch (std::exception const &e) {}
			hough_circles = true;
		}

		// if -g [x] apply gaussian blur with specified kernel size
		else if (!strcmp(argv[i], "-g") && !gaussian)  {
			try { gaussian_val = std::stoi(argv[i+1]); i++; }
			catch (std::exception const &e) {}
			gaussian = true;
		}

		// if -t [x] apply thresholding to gradient magnitudes at specified value
		else if (!strcmp(argv[i], "-t") && !threshold) {
			try { threshold_val = std::stoi(argv[i+1]); i++; }
			catch (std::exception const &e) {}
			threshold = true;
		}
		
		// unrecognised argument
		else { 
			std::cout << "\nError: check your flags! ('?' for help)\n" << std::endl; 
			return -1; }

	}
	
    // cannot threshold magnitudes without running sobel edge detector first
	if (threshold && !sobel) { 
        std::cout << "\nError: magnitude threshold requires sobel edge detection (-s)!\n" << std::endl; 
        return -1; 
    }

    // cannot calculate hough space without magnitudes from sobel edge detection
	else if (hough_circles && !sobel) { 
        std::cout << "\nError: hough transform requires sobel edge detection (-s)!\n" << std::endl;     
        return -1; 
    }

    // return pass
    return 1;
}

// writes contents of args_help.txt
void ArgsHelper() {

    // open file
    string file_name = "args_help.txt";
    std::ifstream f(file_name);

    // output file contents
    std::cout<<"\n"<<std::endl;
    if (f.is_open()) std::cout << f.rdbuf();
    std::cout<<"\n"<<std::endl;

}