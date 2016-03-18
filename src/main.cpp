#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <utility>
#include <algorithm>
#include <thread>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "Utils.hpp"
#include "SimpleLateFusion.hpp"
#include "SiftExtractor.hpp"

using namespace std;
using namespace cv;


vector< pair<int,int> > extractKeyframes(string kfPath) {
	vector< pair<int,int> > keyframes = Utils::parseCSV(kfPath);
	sort(keyframes.begin(),keyframes.end(),Utils::pairCompare);
	//Check if the first shot is "0". If it's not, make it so...
	if(keyframes[0].first > 0) {
		Utils::normalizePairs(keyframes,-(keyframes[0].first));
	}
	
	return keyframes;
}

int main(int argc, char* argv[]) {
	string vPath = "/home/tiago/basevideos/doutorado_audio/iceage_puro_com_audio.mp4";
	string kfPath = "/home/tiago/github/iceage_tomadas_novo.csv";
	string auralDescriptorsFolder = "/home/tiago/basevideos/mfcc_iceage/";	
	bool generateTempFiles = true;
	bool verbose = true;
	vector< pair<int,int> > keyframes = extractKeyframes(kfPath);
	
	if(!verbose) {
		cout.setstate(std::ios_base::failbit);
	}
	
	
	int vDicSize = 500;
	int aDicSize = 50;
	
	SimpleLateFusion slf(vPath, vDicSize, aDicSize, keyframes, auralDescriptorsFolder, generateTempFiles);
	slf.execute();
	/*
	SimpleLateFusion slf(vPath, 500, 500, keyframes);
	
	vector<int> desiredFrames;
	

	for(pair<int,int> kf : keyframes) {
		desiredFrames.push_back(kf.second);
	}
	
	SiftExtractor sift(vPath, nThreads, desiredFrames);
	sift.extract();
	
	vector<Mat> visualDescriptors = sift.getDescriptors();		
	Mat visualDictionary = Utils::kmeansClustering(visualDescriptors, visualDictionarySize);
*/
}
