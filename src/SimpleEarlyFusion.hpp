#ifndef SIMPLEEARLYFUSION_H
#define SIMPLEEARLYFUSION_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
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

class SimpleEarlyFusion {
	private:
		string videoPath;
		int vDicSize, aDicSize;
		vector< pair<int,int> > keyframes;
		string aDescFolder;
		bool tempFiles;
		
		vector<Mat> parseAuralDescriptors();
		vector< vector<double> > createHistograms(vector<Mat> descriptors, Mat dictionary);
		
	public:
		SimpleEarlyFusion(string videoPath, int vDicSize, int aDicSize, vector< pair<int,int> > keyframes, string aDescFolder, bool tempFiles);	
		void execute();
		
		
};

#endif
