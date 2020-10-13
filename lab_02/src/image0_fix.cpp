#include <stdio.h>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

using namespace cv;

int main() {

  // Read image from file
  Mat image = imread("resources/mandrill0.jpg", 1);

  // Threshold by looping through all pixels
  for(int y=0; y<image.rows; y++) {
   for(int x=0; x<image.cols; x++) {

     uchar pixelBlue = image.at<Vec3b>(y,x)[0];
     uchar pixelGreen = image.at<Vec3b>(y,x)[1];
     uchar pixelRed = image.at<Vec3b>(y,x)[2];

     image.at<Vec3b>(y,x)[0]=pixelBlue;
     image.at<Vec3b>(y,x)[1]=pixelGreen;
     image.at<Vec3b>(y,x)[2]=pixelRed+50;

     // if(pixelRed+20>255) { image.at<Vec3b>(y,x)[2]=255; }
     // else { image.at<Vec3b>(y,x)[2]=pixelRed+20; }

   }
 }

  //Save thresholded image
  imwrite("out/image0_fix.jpg", image);

  return 0;
}
