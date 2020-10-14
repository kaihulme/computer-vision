/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - thr.cpp
// TOPIC: basic thresholding
//
// Getting-Started-File for OpenCV
// University of Bristol
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

#define HIGH_THRESHOLD 64
#define LOW_THRESHOLD 32

using namespace cv;

int main() {

  // Read image from file
  Mat image = imread("resources/mandrill.jpg", 1);

  // Convert to grey scale
  Mat gray_image;
  cvtColor(image, gray_image, CV_BGR2GRAY);

  // Threshold by looping through all pixels
  for (int y = 0; y<gray_image.rows; y++) {
    for (int x = 0; x<gray_image.cols; x++) {

      uchar pixel = gray_image.at<uchar>(y, x);

      if (pixel>HIGH_THRESHOLD) gray_image.at<uchar>(y, x) = 255;   // white if pixel value above threshold
      else if (pixel<LOW_THRESHOLD) gray_image.at<uchar>(y, x) = 0; // black if pixel value below threshold

    }
  }

  //visualise the loaded image in the window
  imshow("Display window", gray_image);

  //wait for a key press until returning from the program
  waitKey(0);

  //Save thresholded image
  imwrite("out/thr.jpg", gray_image);

  return 0;
}
