#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <thread>
#include <vector>

using namespace cv;
using namespace std;

class RealTimeVideo {

	VideoCapture* capture;
	Mat* buffer;
	vector<Mat> frameBuffer;
	int buffLen, sampleLen;
	int pos, newpos;
	float frameRate;
	vector<Point> centroids;
	Mat frame;
	vector<float> freqBins;
	vector<float> freqs;
	vector<Mat> processedFrames;

	bool putFrameInBuffer(Mat& f);


public:


	Point pt;

	RealTimeVideo();
	RealTimeVideo(int buffLen, int pSampleLen, vector<float> freq, VideoCapture* capture);

	void producer();
	void processor();
	void UI();
	void runThreads();

	void processSample(vector<Mat>);
	vector<Mat> getSample();
	Mat reshapeSample(vector<Mat>);
	void showFrameOutput();
	void showProcessedOutput();
	Point findCentroid(Mat);

};