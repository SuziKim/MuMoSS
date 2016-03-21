#ifndef UTILS_H
#define UTILS_H

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
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

class Utils {
	private:
		static double euclideanDistance(Mat d1, Mat d2);
		static double euclideanDistance(vector<double> d1, vector<double> d2);
		static double minEuclideanDistance(vector< vector<double> > h1, vector< vector<double> > h2);
		static void extractBoFHistogram(vector<double> &histogram, Mat &descriptor, Mat &dictionary);
		
	public:
		static Mat ArmadilloKmeansClustering(vector<Mat> descriptors, int nClusters);
		static Mat kmeansClustering(vector<Mat> descriptors, int nClusters);
		static void writeOutputFile(string outFile, vector< pair<int,int> > csv);
		static bool checkFile(string name);
		static bool checkFolder(string name);
		static vector< pair<int,int> > parseCSV(string filePath);
		static void normalizePairs(vector< pair<int,int> > &pairs, int val);
		static bool pairCompare(const pair<int,int> &a, const pair<int,int> &b);
		static Mat parseCSVDescriptor(string filePath);
		static vector<Mat> parseAuralDescriptors(string folder);		
		static void writeCSVMat(string file, Mat data);
		static void writeCSVVector(string file, vector< vector<double> > data);
		static string calculateMD5(string file);
		static vector< vector<double> > parseCSVHistograms(string filePath);
		static vector< vector< vector<double> > > histogramsConcat(vector< vector<double> > vHist, vector< vector<double> > aHist, vector< pair<int,int> > keyframes);
		static vector< pair<int,int> > sceneSegmentation(int windowsSize, double simFactor, vector< vector< vector<double> > > shots, vector< pair<int,int> > keyframes);	
		static vector< vector< vector<double> > > generateShotsFromHistogram(vector< vector<double> > hist, vector< pair<int,int> > keyframes);
		static vector< vector<double> > createHistogramsFromDescriptors(vector<Mat> descriptors, Mat dictionary);
};

#endif
