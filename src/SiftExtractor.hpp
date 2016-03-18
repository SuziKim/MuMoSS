#ifndef SIFTEXTRACTOR_H
#define SIFTEXTRACTOR_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <utility>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <thread>


using namespace std;
using namespace cv;

class SiftExtractor {
	private:
		vector<int> desiredFrames;
		string videoPath;
		int numThreads;
		vector<Mat> siftDescriptors;
		void extractSift(Mat &out, Mat frame);
		

	public:
		vector<Mat> getDescriptors();
		SiftExtractor(string videoPath, int numThreads, vector<int> desiredFrames);
		void extract();
};

#endif
