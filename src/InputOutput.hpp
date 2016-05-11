#ifndef INPUTOUTPUT_H
#define INPUTOUTPUT_H

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <iomanip>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cmath>
#include <armadillo>
#include <openssl/md5.h>
#include <thread>

using namespace std;
using namespace cv;

class InputOutput {
	public:	
		static Mat parseCSVDescriptor(string filePath);

		static void writeCSV(string file, vector< pair<int,int> > csv);
		static void writeCSV(string file, Mat data);
		static void writeCSV(string file, vector< vector<double> > data);
		
		static bool checkFile(string name);
		static bool checkFolder(string name);
		
		static vector< pair<int,int> > parseCSV(string filePath);
		static vector< vector<double> > parseCSVHistograms(string filePath);
		static vector<Mat> parseAuralDescriptors(string folder);
		
		static string calculateMD5(string file);


};

#endif
