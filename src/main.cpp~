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
#include "SimpleEarlyFusion.hpp"
#include "SiftExtractor.hpp"
#include "InputOutput.hpp"

using namespace std;
using namespace cv;


vector< pair<int,int> > extractKeyframes(string kfPath) {
	vector< pair<int,int> > keyframes = InputOutput::parseCSV(kfPath);
	sort(keyframes.begin(),keyframes.end(),Utils::pairCompare);
	//Check if the first shot is "0". If it's not, make it so...
	if(keyframes[0].first > 0) {
		Utils::normalizePairs(keyframes,-(keyframes[0].first));
	}
	
	return keyframes;
}

bool isKeyFramesValid(vector< pair<int,int> > kf) {
	int shots = kf.back().first;
	bool temp;
	
	for(int i = 0; i < shots; i++) {
		bool temp = false
		for(pair<int, int> p : kf) {
			if(p.first == i) {
				temp = true;
				break;
			}
		}
		if(!temp) {
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[]) {
	
	if(argc < 7 || argc > 9) {
		cout << "Incorrect parameter count." << endl;
		cout << "./MuMoSS <videoFilePath> <keyframesFilePath> <auralDescriptorsFolder> <algorithm> <visualDictionarySize> <auralDictionarySize> [<useTemporaryFiles> <verbose>]" << endl;
		cout << "Example: " << endl;
		cout << "./MuMoSS video.avi keyframes.csv audio_features/ slf 100 25 yes no" << endl;
		return 1;
	}
	
	string vPath = string(argv[1]);
	if(!InputOutput::checkFile(vPath)) {
		cout << "The videoFilePath seems to be invalid or cannot be read" << endl;
		return 1;
	}
	
	string kfPath = string(argv[2]);
	if(!InputOutput::checkFile(kfPath)) {
		cout << "The keyframesFilePath seems to be invalid or cannot be read" << endl;
		return 1;
	}
	
	string auralDescriptorsFolder = string(argv[3]);
	if(!InputOutput::checkFolder(auralDescriptorsFolder)) {
		cout << "The auralDescriptorsFolder could not be found" << endl;
		return 1;
	}
	
	int vDicSize = atoi(argv[5]);
	if(vDicSize <= 0) {
		cout << "The visualDictionarySize value is invalid" << endl;
		return 1;	
	}
	int aDicSize = atoi(argv[6]);
	if(aDicSize <= 0) {
		cout << "The auralDictionarySize value is invalid" << endl;
		return 1;	
	}
		
	bool tempFiles = true;	
	if(argc > 7) {
		string strTempFile = string(argv[7]);
		if(strTempFile == "No" || strTempFile == "no" || strTempFile == "NO" || strTempFile == "nO") {
			tempFiles = false;
		}
	}
	
	if(argc > 8) {
		string strV = string(argv[8]);
		if(strV == "No" || strV == "no" || strV == "NO" || strV == "nO") {
			cout.setstate(std::ios_base::failbit);
		}
	}
	
	string algorithm = string(argv[4]);
	vector< pair<int,int> > keyframes = extractKeyframes(kfPath);
	/* Check if there is a keyframe for each shot */
	if(!isKeyFramesValid(keyframes)) {
		cout.clear();
		cout << "There seems to be some shots without a selected keyframe!" << endl;
		return 1;
	}
	
	
	if(algorithm == "slf" || algorithm == "SLF") {
		cout << "Running the Simple Late Fusion algorithm" << endl;
		SimpleLateFusion slf(vPath, vDicSize, aDicSize, keyframes, auralDescriptorsFolder, tempFiles);
		slf.execute();
	} else {
		if(algorithm == "sef" || algorithm == "SEF") {
			cout << "Running the Simple Early Fusion algorithm" << endl;
			SimpleEarlyFusion sef(vPath, vDicSize, aDicSize, keyframes, auralDescriptorsFolder, tempFiles);
			sef.execute();
		} else {
			cout << "Algorithm '" << algorithm << "' not found!" << endl;
		}
	}
	

}
