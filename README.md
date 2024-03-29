# computer-vision

Implementation of traditional computer vision techniques using C++ and the OpenCV library.

## requirements
- Requires OpenCV and make.
- Development has been done from within a VirtualBox using Vagrant:
  - other set-ups may need make configuring differently.
  - please change `$LIBS` in the makefile to point to your OpenCV libraries.

### build and run
- Run `make` to compile component.
- Arguments for each component below.
- Place target images in `/resources`.
- Find output in `/out`.

#### argument help:
- the first argument should be the file to be transformed
    - ignore image file extension (.png, .jpg etc.)
    - image should be places in /resources
- (-g [x]) will apply a gaussian blur:
    - x specifies the kernel size
    - not specifying x will deafult to kernel size 3
    - applying a blur will remove some noise and improve performance of the edge detector
    - this will output [file]_gaussian to /out
- (-s) will apply the sobel edge detection:
    -this will output the following to /out
        - [file]_dfdx (horizontal gradients)
        - [file]_dfdy (vertical gradients)
        - [file]_magnitude (magnitude of gradients: ∇|f(x,y)| = sqrt( (df/dx)^2 + (df/dy)^2 ) )
        - [file]_direction (angle of gradients: φ = arctan( (df/dy) / (df/dx) ) )
- (-h [min][max][r_step][t_step]) will apply hough transform circles:
    - min specifies the minimum radius for the hough transform circles
        - not specifying min will default to 10
        - this size may not work on your image so chance acoordingly
    - max specifies the maximum radius for the hough transform circles
        - not specifying max will default to 100
        - this size may not work on your image so chance acoordingly
    - r_step specifies the step between each hough transform radii
        - not specifying r_step will default to 1
        - increasing should improve processing time at the cost of accuracy
    - t_step specifies the step between the angles the hough transform circles are drawn
        - not specifying t_step will default to 1
        - increasing should improve processing time at the cost of accuracy
- (-t [x]) will threshold the values of the gradient magnitudes:
    - this will output [file]_thresholded to /out
    - x is the value to threshold at: lower will be set to black, equal to or higher to white
        - not specifying x will default to 100
    - check the output image and adjust x to highlight edges for improved hough transform
