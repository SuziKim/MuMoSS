#ifndef SIFTEXTRACTOR_H
#define SIFTEXTRACTOR_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <utility>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <thread>


using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

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
