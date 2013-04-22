// write a function which determines if a given x,y point is in
// the mandelbrot set or not

// Iterate until:
// until you generate a point Pn which is 2 or more away from the origin,
// or until you have performed 256 iterations

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0

#define MAX_ITERATIONS 256

int isInMandelbrot (double x, double y);
int mandelbrotEscapeTime (int maxIterations, double x, double y);

int main (int argc, char* argv[]) {
    double testPointX, testPointY;
    
    assert(isInMandelbrot(0,1) == TRUE);
    assert(isInMandelbrot(0,0) == TRUE);
    assert(isInMandelbrot(-1,0) == TRUE);
    assert(isInMandelbrot(-1,1) == FALSE);
    assert(isInMandelbrot(5,0) == FALSE);
    assert(isInMandelbrot(0,2) == FALSE);
    assert(isInMandelbrot(3,0) == FALSE);
    assert(isInMandelbrot(1,0) == FALSE);
    assert(isInMandelbrot(1,1) == FALSE);
    
    printf("Enter the points to check as x,y: ");
    scanf("%lf,%lf", &testPointX, &testPointY);
    
    if (isInMandelbrot(testPointX, testPointY) == TRUE) {
        printf("The point is in the set.\n");
    } else {
        printf("The point is not in the set.\n");
    }

    return EXIT_SUCCESS;
}

int isInMandelbrot (double x, double y) {
    int result;
    
    if (mandelbrotEscapeTime (MAX_ITERATIONS, x, y) == MAX_ITERATIONS) {
        result = TRUE;
    } else {
        result = FALSE;
    }
    
    return result;
}

int mandelbrotEscapeTime (int maxIterations, double x, double y) {
    // (x, y)^2 = (x + yi)^2
    //          = x^2 + 2xyi + y^2 * i^2
    //          = x^2 - y^2 + 2xyi          [i^1 = -1]
    //          = (x^2 - y^2) + (2xyi)
    //          = (x^2 - y^2, 2xy)
    int iteration;
    double calcX, calcY, calcTempX;
    
    calcX = 0;
    calcY = 0;
    iteration = 0;
    while ((iteration < maxIterations) &&
            (calcX * calcX + calcY * calcX < 2 * 2)) {
        calcTempX = (calcX * calcX - calcX * calcX) + x;
        calcY = (2 * calcX * calcX) + y;
        calcX = calcTempX;
    
        iteration++;
    }
    
    return iteration;
}







