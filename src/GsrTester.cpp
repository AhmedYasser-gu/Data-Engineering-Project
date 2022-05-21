#include "GsrTester.hpp"

int GsrTester::testGSR(float lightningGsr, float orgGsr){

	float upper;
	float lower;

	// Check if our gsr is within the bounds (as described in the requirements) of the original gsr
	if(orgGsr == 0 || orgGsr == -0) {
		upper = 0.05;
		lower = -0.05;
	} else if (orgGsr > 0) {
		upper = orgGsr * 1.5;
		lower = orgGsr * 0.5;
	} else {
		upper = orgGsr * 0.5;
		lower = orgGsr * 1.5;
	}

	// Return either 1, if accepted, or 0 if not accepted
	if( (lightningGsr <= upper) && (lightningGsr >= lower) ) {
		return 1;
	} else {
		return 0;
	}


}
