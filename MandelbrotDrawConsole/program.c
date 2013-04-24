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

#define DRAWING_WIDTH 80
#define DRAWING_HEIGHT 72
#define DRAWING_ZOOM 5

int isInMandelbrot (double x, double y);
int mandelbrotEscapeTime (int maxIterations, double x, double y);
// dealing with non-integer exponents is difficult and unecessary
double power(int base, int exponent);

int main (int argc, char* argv[]) {
    int drawingPointX;
    int drawingPointY;
    
    double scaledPointX, scaledPointY;
    double scale;
    
    assert(isInMandelbrot(0,1) == TRUE);
    assert(isInMandelbrot(0,0) == TRUE);
    assert(isInMandelbrot(-1,0) == TRUE);
    assert(isInMandelbrot(-1,1) == FALSE);
    assert(isInMandelbrot(5,0) == FALSE);
    assert(isInMandelbrot(0,2) == FALSE);
    assert(isInMandelbrot(3,0) == FALSE);
    assert(isInMandelbrot(1,0) == FALSE);
    assert(isInMandelbrot(1,1) == FALSE);
    
    scale = 1 / power(2, DRAWING_ZOOM);
    
    drawingPointY = 0;
    while (drawingPointY < DRAWING_HEIGHT) {
        drawingPointX = 0;
        while (drawingPointX < DRAWING_WIDTH) {
            // centering
            scaledPointX = -(DRAWING_WIDTH / 2) * scale;
            scaledPointY = -(DRAWING_HEIGHT / 2) * scale;
            
            // calculate current point
            scaledPointX += drawingPointX * scale;
            scaledPointY += drawingPointY * scale;
            //printf("%d, %d\n", drawingPointX, drawingPointY);
            //printf("%lf, %lf\n", scaledPointX, scaledPointY);
            if (isInMandelbrot(scaledPointX, scaledPointY) == TRUE) {
                printf("*");
            } else {
                printf(" ");
            }
            
            drawingPointX++;
        }
        printf("\n");
        drawingPointY++;
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
            (calcX * calcX + calcY * calcY < 2 * 2)) {
        calcTempX = (calcX * calcX - calcY * calcY) + x;
        calcY = (2 * calcX * calcY) + y;
        calcX = calcTempX;
    
        iteration++;
    }
    
    return iteration;
}

double power(int base, int exponent) {
    int i = 0;
    double calcResult = 1;
    while (i < exponent) {
        calcResult *= base;
        i++;
    }
    
    return calcResult;
}





