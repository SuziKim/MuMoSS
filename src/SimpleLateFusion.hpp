#ifndef SIMPLELATEFUSION_H
#define SIMPLELATEFUSION_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <future>
#include <thread>
#include <stdlib.h>
#include <cmath>

#include "SiftExtractor.hpp"
#include "Utils.hpp"
#include "InputOutput.hpp"

using namespace std;
using namespace cv;

class SimpleLateFusion {
	private:
		string videoPath;
		int vDicSize, aDicSize;
		vector< pair<int,int> > keyframes;
		string aDescFolder;
		bool tempFiles;
		
		vector<Mat> parseAuralDescriptors();
		vector< pair<int,int> > scenesFusion(vector< pair<int,int> > visualScenes, vector< pair<int,int> > auralScenes);
		
	public:
		SimpleLateFusion(string videoPath, int vDicSize, int aDicSize, vector< pair<int,int> > keyframes, string aDescFolder, bool tempFiles);	
		void execute();
		
		
};

#endif
