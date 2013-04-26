#define MAX_ITERATIONS 256

unsigned char stepsToRed (int steps) {
    return 0xFF - (0xFF * steps / MAX_ITERATIONS);
}
unsigned char stepsToBlue (int steps) {
    return 0xFF - (0xFF * steps / MAX_ITERATIONS);
}
unsigned char stepsToGreen (int steps) {
    return 0xFF - (0xFF * steps / MAX_ITERATIONS);
}
