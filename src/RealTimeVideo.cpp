#define _USE_MATH_DEFINES //M_PI

#include "RealTimeVideo.h"
#include <complex>
#include <limits.h>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace cv;
using namespace std;


RealTimeVideo::RealTimeVideo() {


}

RealTimeVideo::RealTimeVideo(int buffLen, int sampleLen, vector<float> freqs, VideoCapture* capture)
: buffLen(buffLen), sampleLen(sampleLen), freqs(freqs), capture(capture)
{
	frameRate = capture->get(cv::CAP_PROP_FPS);
	for (int i = 0; i < buffLen; i++) {
		frameBuffer.push_back(Mat(240, 320, CV_8U, cv::Scalar::all(0)));
	}

	for (int i = 0; i < freqs.size(); i++) {
		processedFrames.push_back(Mat(240, 320, CV_8U, cv::Scalar::all(0)));
	}
	centroids = vector<Point>(freqs.size(), Point(0, 0));

	frame = Mat(240, 320, 0, cv::Scalar::all(0));

	freqBins.clear();
	for (int i = 0; i < sampleLen / 2; i++) {
		freqBins.push_back(i * frameRate / sampleLen);
		//cout << i * frameRate / sampleLen << endl;
	}

	cerr << "\nInitialized RealtimeVideo\n";

}

bool RealTimeVideo::putFrameInBuffer(Mat& f) {

	frameBuffer[pos] = f.clone();
	pos = (pos+1) % buffLen;

	return true;

}

void RealTimeVideo::producer() {

	cerr << "\nStarting Producer Thread\n";

	waitKey(500);

	pos = 0;
	Mat f, gray;

	while (1) {

		capture->read(f);


		cvtColor(f, gray, COLOR_BGR2GRAY);
		frame = gray.clone();
		putFrameInBuffer(gray);

		waitKey(10);
	}
}

void RealTimeVideo::showFrameOutput() {

	imshow("Webcam", frame);


}

void RealTimeVideo::showProcessedOutput() {

	for (int i = 0; i < freqs.size(); i++) {
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << freqs[i];
		std::string s = stream.str();
		imshow("Processed Output " + s + " Hz", processedFrames[i]);
	}



}
void RealTimeVideo::UI() {

	cerr << "\nStarting UI Thread\n";
	waitKey(1000);
	while (true) {

		if (waitKey(50) >= 0) break;

		try {
			showProcessedOutput();
			showFrameOutput();
		}
		catch (exception& e) {

		}
	}

}

void RealTimeVideo::runThreads() {

	thread producer_t(&RealTimeVideo::producer, this);

	thread UI_t(&RealTimeVideo::UI, this);

	thread processor_t(&RealTimeVideo::processor, this);

	cerr << "\nLaunched Threads\n";

	waitKey(0);
	producer_t.join();

}

void RealTimeVideo::processor() {

	cerr << "\nStarting Processor Thread\n";
	vector<Mat> sample;
	waitKey(1000);
	while (1) {

		sample = getSample();
		processSample(sample);

	}

}

vector<Mat> RealTimeVideo::getSample() {

	if (frameBuffer.size()> sampleLen){
		int end = pos;
		int st = pos - sampleLen;
		if (st < 0) {
			st += buffLen;
			//st = 0;
			vector<Mat> a(frameBuffer.begin() + st, frameBuffer.end());
			vector<Mat> b(frameBuffer.begin(), frameBuffer.begin() + end);
			a.insert(a.end(), b.begin(), b.end());
			return a;
		}
		else {
			vector<Mat> ret(frameBuffer.begin() + st, frameBuffer.begin() + end);
			return ret;
		}
	}
	return {Mat(240, 320, CV_8U, cv::Scalar::all(0))};

}

Mat RealTimeVideo::reshapeSample(vector<Mat> sample) {

	cv::Mat ret(sample.back().rows*sample.back().cols, sample.size(), sample.back().type(), cv::Scalar::all(0));


}

void RealTimeVideo::processSample(vector<Mat> sample) {
	if (sample.size() < sampleLen) {
		waitKey(100);
		return;
	}


	int64 e1 = getTickCount();

	//vector<Mat> pFrames(freqs.size(), Mat(240, 320, CV_32FC1, cv::Scalar::all(0)));
	vector<Mat> pFrames;
	for (int i = 0; i < freqs.size(); i++) {
		pFrames.push_back(Mat(240, 320, CV_32FC1, cv::Scalar::all(0)));
	}

	for (int row = 0; row < sample.back().rows; row++) {
		for (int col = 0; col < sample.back().cols; col++) {
			vector<float> fft(sampleLen,0.0);

			for (int t = 0; t < sample.size(); t++) {

				//Hanning window
				float wn = 0.5 * (1 - cos(2 * M_PI * t / sampleLen));
				//float wn = 1.0f;

				fft[t] = wn*(float(sample[t].at<uchar>(row, col)));

			}
			dft(fft, fft);

			for (int i = 0; i < freqs.size(); i++) {
				int n = int(std::round(freqs[i] / (frameRate / sampleLen)));
				std::complex<float> mycomplex(fft[(n * 2) - 1], fft[(n * 2)]);
				float mag = std::abs(mycomplex);
				pFrames[i].at<float>(row, col) = mag;
			}


			//// 1Hz
			//float freq = 1.0;
			//int n = int(std::round(freq / (frameRate / sampleLen)));

			////vector<float> bins(sampleLen/2);

			//std::complex<float> mycomplex(fft[(n * 2) - 1], fft[(n * 2)]);
			//float mag = std::abs(mycomplex);
			////float mag = sqrtf(powf(fft[(n * 2) - 1], 2.0f) + powf(fft[(n * 2)], 2.0f));
			//pFrame1.at<float>(row, col) = mag;

			//// 5 Hz
			//freq = 5.15625;
			//n = int(std::round(freq / (frameRate / sampleLen)));

			//mycomplex = std::complex<float>(fft[(n * 2) - 1], fft[(n * 2)]);
			//mag = std::abs(mycomplex);

			////mag = sqrtf(powf(fft[(n * 2) - 1], 2.0f) + powf(fft[(n * 2)], 2.0f));
			//pFrame2.at<float>(row, col) = mag;
		}
	}


	for (int i = 0; i < freqs.size(); i++) {
		double min, max;
		cv::minMaxLoc(pFrames[i], &min, &max);

		pFrames[i] *= 255 / float(max);
		pFrames[i].convertTo(pFrames[i], CV_8U);

		centroids[i] = findCentroid(pFrames[i]);
		cvtColor(pFrames[i], pFrames[i], COLOR_GRAY2BGR);
		circle(pFrames[i], centroids[i], 5, Scalar(128, 0, 0), -1);

		processedFrames[i] = pFrames[i].clone();
	}


	int64 e2 = getTickCount();
	double time = (e2 - e1) / getTickFrequency();

	cout << "time: " << time << "s" << endl;

	//cout << "Height : " << processedFrame.rows << endl;
	//cout << "Width: " << processedFrame.cols << endl;
	//cout << "Width: " << unsigned int(processedFrame.at<uchar>(0,0)) << endl;
}

Point RealTimeVideo::findCentroid(Mat sample) {

	Mat thr;
	threshold(sample, thr, 100, 255, THRESH_BINARY);

	Moments m = moments(thr, true);
	Point p(int(m.m10 / m.m00), int(m.m01 / m.m00));

	return p;

}
