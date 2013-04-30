#define MAX_ITERATIONS 256
#define FIRST_STEP (4 * MAX_ITERATIONS / 5)
#define SECOND_STEP (2 * MAX_ITERATIONS / 5)

unsigned char stepsToBlue (int steps) {
    unsigned char result;
    if (steps == MAX_ITERATIONS) {
        result = 0x00;
    } else if (steps > FIRST_STEP) {
        result = 0xFF * steps / MAX_ITERATIONS * 2;
    } else {
    	result = 0xAA * MAX_ITERATIONS / steps;
    }
    
    return result;
}
unsigned char stepsToRed (int steps) {
    unsigned char result;
    if (steps == MAX_ITERATIONS) {
        result = 0x00;
    } else if (steps > SECOND_STEP) {
        result = 0xCC * steps / MAX_ITERATIONS;
    } else {
        result = 0x44 * steps / MAX_ITERATIONS;
    }
    return result;
}
unsigned char stepsToGreen (int steps) {
    unsigned char result;
    if (steps == MAX_ITERATIONS) {
        result = 0x00;
    } else if (steps > FIRST_STEP) {
    	result = 0xFF * steps / MAX_ITERATIONS * 2;
    } else if (steps > SECOND_STEP) {
    	result = 0x77 * steps / MAX_ITERATIONS;
    } else {
    	result = 0x33 * 60 / steps;
    }
    	
    return result;
}

/*unsigned char stepsToRed (int steps) {
    if (steps < 200) {
    	return 0xFF * steps / MAX_ITERATIONS * 4;
    } else {
    	return 0xFF * steps / MAX_ITERATIONS;
    }
}
unsigned char stepsToBlue (int steps) {
    if (steps > 200) {
    	return 0xFF * steps / MAX_ITERATIONS * 0;
    } else if (steps < 100) {
    	return 0xAA * steps / MAX_ITERATIONS;
    } else {
    	return 0x00;
    }
}
unsigned char stepsToGreen (int steps) {
    if (steps < 200) {
    	return 0xFF * steps / MAX_ITERATIONS * 1/3;
    } else {
    	return 0xFF * 4 * steps / MAX_ITERATIONS;
    }
    	
}*/


/*#define MAX_ITERATIONS 256

unsigned char stepsToRed (int steps) {
    return (0xFF * 1/2 * steps / MAX_ITERATIONS);
}
unsigned char stepsToBlue (int steps) {
    return (0xFF * 3 * steps / MAX_ITERATIONS);
}
unsigned char stepsToGreen (int steps) {
    return (0xFF * 1/4 * steps / MAX_ITERATIONS);
}*/
