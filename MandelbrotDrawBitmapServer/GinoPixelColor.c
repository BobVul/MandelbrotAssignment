#define MAX_ITERATIONS 256


unsigned char stepsToBlue (int steps) {
    if (steps > 200 && steps < 256 ) {
      return 0xFF * steps / MAX_ITERATIONS * 2;
    } else if (steps >= MAX_ITERATIONS) {
    	return 0x22;
    } else {
    	return 0xaa;
    }
}
unsigned char stepsToRed (int steps) {
if (steps < 20) {
	return 0x00;
}
return 0xcc;
}
unsigned char stepsToGreen (int steps) {

    if (steps > 200 && steps < 256 ) {
    	return 0xFF * steps / MAX_ITERATIONS * 2;
    } else if (steps >= MAX_ITERATIONS) {
    	return 0x55;
    } else if (steps < 100) {
    	return 0xFF * steps / 30;
    } else {
    	return 0xff;
    }
    	
    	
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
