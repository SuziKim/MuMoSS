#include "SiftExtractor.hpp"

SiftExtractor::SiftExtractor(string videoPath, int numThreads, vector<int> desiredFrames) {
	this->videoPath = videoPath;
	this->desiredFrames = desiredFrames;
	for(int i = 0; i < desiredFrames.size(); i++) {
		this->siftDescriptors.push_back(Mat());
	}
	if(numThreads <= 0) {
		cout.clear();
		cout << "SiftExtractor: The number of threads cannot be less than 1!" << endl;
		exit(-1);
	}
	this->numThreads = numThreads;
}

vector<Mat> SiftExtractor::getDescriptors() {
	return this->siftDescriptors;
}

void SiftExtractor::extractSift(Mat &out, Mat frame) {
	Mat gray;	
	
	SiftFeatureDetector detector;
	SiftDescriptorExtractor extractor;	
	vector<KeyPoint> kp;
	
	cvtColor(frame,gray,CV_BGR2GRAY);
	detector.detect(gray,kp);
	extractor.compute(gray,kp,out);
	
	frame.release();
	gray.release();
	kp.clear();
}

void SiftExtractor::extract() {
	VideoCapture video(videoPath);
	if(!video.isOpened()) {
		cout.clear();
		cout << "SiftExtractor: Error opening the video file" << endl;
		exit(-1);
	}
	
	int fNum = 0;
	int frameIndex = 0;
	vector<thread> pool;
	
	Mat frame;
	while(true) {		
		bool status = video.read(frame);
		if(!status || frameIndex >= desiredFrames.size()) {
			break;
		}
		if(fNum == desiredFrames[frameIndex]) {
			//There is enough running threads already?		
			if(pool.size() >= numThreads) {
				//There is! Wait then to complete...
				for(int i = 0; i < numThreads; i++) {
					pool[i].join();
				}
				pool.clear();
			}
			pool.push_back(thread(&SiftExtractor::extractSift,this,std::ref(this->siftDescriptors[frameIndex]),frame));
			frameIndex++;
		}
		fNum++;
	}
	for(int i = 0; i < pool.size(); i++) {
		pool[i].join();
	}
	pool.clear();
	frame.release();
	video.release();	
}


