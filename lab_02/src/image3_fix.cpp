#include <stdio.h>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

using namespace cv;

int main() {

  // Read image from file
  Mat image = imread("resources/mandrill3.jpg", 1);

  // Threshold by looping through all pixels
  for(int y=0; y<image.rows; y++) {
   for(int x=0; x<image.cols; x++) {

     uchar pixelBlue = image.at<Vec3b>(y,x)[0];
     uchar pixelGreen = image.at<Vec3b>(y,x)[1];
     uchar pixelRed = image.at<Vec3b>(y,x)[2];

     // invert red green and blue pixels
     image.at<Vec3b>(y,x)[0]=pixelBlue*2;
     image.at<Vec3b>(y,x)[1]=pixelGreen*2;
     image.at<Vec3b>(y,x)[2]=pixelRed;

   }
  }

  //Save thresholded image
  imwrite("out/image3_fix.jpg", image);

  return 0;
}
