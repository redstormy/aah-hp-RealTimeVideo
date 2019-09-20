// aah_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//! [includes]
#include "RealTimeVideo.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>
//! [includes]

//! [namespace]
using namespace cv;
using namespace std;
//! [namespace]



int main(int argc, const char* argv[])
{

	if (argc > 1) {
		VideoCapture cap(argv[1]); // open the default camera
		cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
		cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
		vector<float> freq = { 1, 5.15625 };
		//vector<float> freq = { 7, 11, 13, 15 };

		RealTimeVideo r(1000, 128, freq, &cap);
		r.runThreads();
	}
	else {
		//VideoCapture cap("vibLong.avi"); // open from file
		VideoCapture cap(0); // open the default camera
		cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
		cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);
		cout << "H: " << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << "W: " << cap.get(cv::CAP_PROP_FRAME_WIDTH) << endl;
		if (!cap.isOpened()) {
			cout << "Error opening video stream or file" << endl;
			return -1;
		}
		vector<float> freq = { 3, 5 , 7};
		//vector<float> freq = { 7, 11, 13, 15 };

		RealTimeVideo r(1000, 128,freq, &cap);
		r.runThreads();
	}

	return 0;
}
