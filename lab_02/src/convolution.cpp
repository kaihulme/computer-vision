#include <stdio.h>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

using namespace cv;

// #define KERNEL_SIZE 3;

int main() {

  // Read image from file
  Mat image = imread("resources/mandrill.jpg", 1);

  int conv[3][3] = {{1,1,1},
                    {1,1,1},
                    {1,1,1}};

  // Threshold by looping through all pixels
  for(int y=0; y<image.rows; y++) {
   for(int x=0; x<image.cols; x++) {

     uchar pixel = image.at<uchar>(y,x);

     printf("(%d, %d): %d\n", y, x, pixel);

     uchar a[3][3] = {{0,0,0},
                      {0,0,0},
                      {0,0,0}};

     // get pixels to be used in convolution
     for(int j=-1; j<2; j++) {
       for(int i=-1; i<2; i++) {

         printf("\ny:%d, x:%d, j:%d, i:%d\n", y, x, j, i);
         printf("(y:%d, x:%d) ~> ", (y+j), (x+i));

         // printf("a(x): %d, a(y): %d\n", x+i, y+j);
         // convolutions on edge use a 0 border
         if( ((y+j)>=0) && ((y+j)<256) && ((x+i)>-1) && ((x+i)<256) ) {

           a[i][j] = image.at<uchar>(y+j, x+i);

           printf("set\n");
         // }
       } else { printf("dont set\n"); }
       }
     }

     if(y==2){return 0;}

     printf("{{%d,%d,%d},\n {%d,%d,%d},\n {%d,%d,%d}}\n\n", a[0][0],a[0][1],a[0][2],a[1][0],a[1][1],a[1][2],a[2][0],a[2][1],a[2][2]);

     int convd_pixel = 0;

     // apply convolution for pixel (i,j) with kernel conv on area a
     for(int j=0; j<3; j++) {
       for(int i=0; i<3; i++) {
         convd_pixel += conv[j][i] * a[j][i];
       }
     }

     image.at<uchar>(y, x) = convd_pixel;

   }
 }

  //Save thresholded image
  imwrite("out/convolution.jpg", image);

  return 0;
}
