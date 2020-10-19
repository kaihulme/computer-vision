#include <stdio.h>
#include <opencv/cv.h>        //you may need to
#include <opencv/highgui.h>   //adjust import locations
#include <opencv/cxcore.h>    //depending on your machine setup

using namespace cv;

// #define KERNEL_SIZE 3;

int main() {

  int debug=0;

  // read image in as opencv matrix
  Mat image;
  
  image = imread("resources/mandrill.jpg", CV_LOAD_IMAGE_UNCHANGED);
  Mat new_image = image;

  float k_v = (float)1/(float)9;

  // printf("\n%.6f", k_v);

  // convolution kernel to be applied
  float conv[3][3] = {{k_v, k_v, k_v},
                      {k_v, k_v, k_v},
                      {k_v, k_v, k_v}};

  // for each pixel in the matrix (image)
  for(int y=0; y<image.rows; y++) {
    for(int x=0; x<image.cols; x++) {
      
      if(debug==1){
        printf("-------------------------\n");
      }

      // get the brightness (value) of the pixel
      uchar pixel = image.at<uchar>(y,x);

      // matrix to be populated with image values to be used in convolution
      uchar a[3][3] = {{0,0,0},
                       {0,0,0},
                       {0,0,0}};

      // for area around pixel, i.e. (-1,-1) to (1,1) for pixel (0,0)
      for(int j=-1; j<2; j++) {
        for(int i=-1; i<2; i++) {
          
          // position in the matrix
          int a_y_img = y+j;
          int a_x_img = x+i;
          // position in a
          int a_j = j+1;
          int a_i = i+1;

          // check the position is on the image
          if( (a_y_img>=0) && (a_y_img<image.rows) && (a_x_img>=0) && (a_x_img<image.cols) ) {
            a[a_j][a_i] = image.at<uchar>(a_y_img, a_x_img);
          } // else keep as 0
        }
      }

      if(debug==1) {
        printf("\n{{%d,%d,%d},\n {%d,%d,%d},\n {%d,%d,%d}}\n\n", a[0][0],a[0][1],a[0][2],a[1][0],a[1][1],a[1][2],a[2][0],a[2][1],a[2][2]);
        printf("{{%.6f,%.6f,%.6f},\n {%.6f,%.6f,%.6f},\n {%.6f,%.6f,%.6f}}\n\n", conv[0][0],conv[0][1],conv[0][2],conv[1][0],conv[1][1],conv[1][2],conv[2][0],conv[2][1],conv[2][2]);
      }

      // pixel value after convolution
      int convd_pixel = 0;

      // for each (j,i) in kernel
      for(int j=-1; j<2; j++) {
        for(int i=-1; i<2; i++) {
          // position in the a matrix
          int a_j = 1-j;
          int a_i = 1-i;
          // position in kernel
          int conv_j = j+1;
          int conv_i = i+1;

          convd_pixel += conv[conv_j][conv_i] * a[a_j][a_i];

          if(debug==1) {
            printf("\nconv[%d][%d] * a[%d][%d]\n", conv_j, conv_i, a_j, a_i);

            printf("%d*%d = %d\n", conv[conv_j][conv_i], a[a_j][a_i], conv[conv_j][conv_i]*a[a_j][a_i]);

            printf("convd_pixel = %d\n", convd_pixel);
          }
        }
      }

      // set new pixel value
      new_image.at<uchar>(y, x) = convd_pixel;
      
      if(debug==1) {      
        printf("\nbefore : (%d, %d): %d\n", y, x, pixel);
        printf("after : (%d, %d): %d\n\n", y, x, convd_pixel);

        if(x==3){return 0;}    
      }
    }
  }

  // write new image
  imwrite("out/convolution_old.jpg", new_image);

  return 0;
}
