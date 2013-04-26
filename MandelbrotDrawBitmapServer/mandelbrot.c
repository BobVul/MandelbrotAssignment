#define MAX_ITERATIONS 256

int escapeSteps (double x, double y) {
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
    while ((iteration < MAX_ITERATIONS) &&
            (calcX * calcX + calcY * calcY < 2 * 2)) {
        calcTempX = (calcX * calcX - calcY * calcY) + x;
        calcY = (2 * calcX * calcY) + y;
        calcX = calcTempX;

        iteration++;
    }

    return iteration;
}
