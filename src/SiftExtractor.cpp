#include "SiftExtractor.hpp"

SiftExtractor::SiftExtractor(string videoPath, vector<int> desiredFrames) {
	this->videoPath = videoPath;
	this->desiredFrames = desiredFrames;
	for(int i = 0; i < desiredFrames.size(); i++) {
		this->siftDescriptors.push_back(Mat());
	}	
	this->numThreads = thread::hardware_concurrency();
}

vector<Mat> SiftExtractor::getDescriptors() {
	return this->siftDescriptors;
}

void SiftExtractor::extractSift(Mat &out, Mat frame) {
	Mat gray;	
	
	Ptr<Feature2D> sift = SIFT::create();
	vector<KeyPoint> kp;
	
	cvtColor(frame,gray,CV_BGR2GRAY);
	sift->detect(gray,kp);
	sift->compute(gray,kp,out);
	
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
				for(auto &t : pool) {
					t.join();
				}
				pool.clear();
			}
			Mat t = Mat();
			frame.copyTo(t);
			pool.push_back(thread(&SiftExtractor::extractSift,this,std::ref(this->siftDescriptors[frameIndex]),t));
			frameIndex++;
		}
		fNum++;
	}
	for(auto &t : pool) {
		t.join();
	}
	pool.clear();
	frame.release();
	video.release();	
}


