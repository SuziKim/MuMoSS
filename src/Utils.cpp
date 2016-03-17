#include "Utils.hpp"

Mat Utils::kmeansClustering(vector<Mat> descriptors, int nClusters){
	Mat samples, labels, dic;
	for(Mat m : descriptors) {
		samples.push_back(m);
	}
	kmeans(samples, nClusters, labels, TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 20, 0.1), 10, KMEANS_PP_CENTERS, dic);
	return dic;
}

void Utils::writeOutputFile(string outFile, vector< pair<int,int> > keyframes) {
	ofstream file (outFile.c_str());
	for(int i = 0; i < keyframes.size(); i++) {
		file << keyframes[i].first << "," << keyframes[i].second << endl;
	}
	file.close();
}

bool Utils::checkFile(string name) {
  	if(FILE *file = fopen(name.c_str(),"r")) {
		fclose(file);
		return true;
	}
	return false;
}

bool Utils::checkOutputFile(string name) {
	if(FILE *file = fopen(name.c_str(),"w")) {
		fclose(file);
		return true;
	}
	return false;	
}

bool Utils::pairCompare(const pair<int,int> &a, const pair<int,int> &b) {
	return (a.first < b.first || (a.first == b.first && a.second < b.second));
}

void Utils::normalizePairs(vector< pair<int,int> > &pairs, int val) {
	for(int i = 0; i < pairs.size(); i++) {
		pairs[i].first = pairs[i].first + val;
		pairs[i].second = pairs[i].second + val;
	}
}

vector< pair<int,int> > Utils::parseCSV(string filePath) {
	ifstream file(filePath.c_str());
	string line;
	string token;
	vector< pair<int,int> > ret;
	
	if(file.is_open()) {
		while(getline(file,line)) {
			istringstream str(line);
			vector<int> temp;
			while(getline(str,token,',')) {
				temp.push_back(atoi(token.c_str()));
			}
			ret.push_back(make_pair(temp[0],temp[1]));
			temp.clear();
		}
	}	
    	file.close();
	return ret;
}

vector<Mat> Utils::parseAuralDescriptors(string folder) {
	if(folder.back() != '/') {
		folder = folder + "/";
	}
	vector<Mat> ret;
	int base = 1;
	if(Utils::checkFile(folder + "audio_0.sift")) {
		base = 0;
	}
	
	while(true) {
		string fTemp = folder + "audio_" + to_string(base);
		if(!Utils::checkFile(fTemp)) {
			break;
		}
		ret.push_back(Utils::parseCSVDescriptor(fTemp));		
		base++;
	}
	return ret;
}

Mat Utils::parseCSVDescriptor(string filePath) {
	ifstream file(filePath.c_str());
	string line;
	string token;
	vector< vector<float> > values;
	
	if(file.is_open()) {
		while(getline(file,line)) {
			istringstream str(line);
			vector<float> temp;
			while(getline(str,token,',')) {
				temp.push_back(atof(token.c_str()));
			}
			values.push_back(temp);
			temp.clear();
		}
	}	
    	file.close();
    	
    	Mat ret(values.size(),values[0].size(), CV_32F);
    	for(int i = 0; i < values.size(); i++) {
    		for(int j = 0; j < values[i].size(); j++) {
    			ret.at<float>(i,j) = values[i][j];
    		}
    	}
    	
	return ret;
}

double Utils::euclideanDistance(Mat d1, Mat d2) {
	double distance = 0.0;
	for(int i = 0; i < d1.cols; i++) {
		distance = distance + pow(d1.at<float>(i) - d2.at<float>(i),2);
	}	
	return sqrt(distance);
}

vector<double> Utils::extractBoFHistogram(Mat descriptor, Mat dictionary) {
	if(descriptor.cols != dictionary.cols) {
		cout << "ERROR! The BoF dictionary and the descriptors should have the same size!" << endl;
		exit(-1); 
	}
	
	vector<double> histogram;
	for(int i = 0; i < dictionary.rows; i++) {
		histogram.push_back(0.0);
	}
	
	for(int i = 0; i < descriptor.rows; i++) {
		Mat desc = descriptor.row(i);
		vector<double> temp;
		for(int j = 0; j < dictionary.rows; i++) {
			Mat dic = dictionary.row(j);
			temp.push_back(Utils::euclideanDistance(desc,dic));
		}
		int indexMin = min_element(temp.begin(), temp.end()) - temp.begin();
		histogram[indexMin]++;
	}
	
	double su = 0.0;
	for(int i = 0; i < histogram.size(); i++) {
		su = su + histogram[i];
	}
	for(int i = 0; i < histogram.size(); i++) {
		histogram[i] = histogram[i] / su;
	}
	return histogram;	
}

void Utils::writeCSVMat(string file, Mat data) {
	ofstream f(file.c_str());
	for(int i = 0; i < data.rows; i++) {
		f << data.at<float>(i,0);
		for(int j = 1; j < data.cols; j++) {
			f << "," << data.at<float>(i,j);
		}
		f << endl;
	}
	f.close();
}

void Utils::writeCSVVector(string file, vector< vector<double> > data) {
	ofstream f(file.c_str());
	for(int i = 0; i < data.size(); i++) {
		f << data[i][0];
		for(int j = 1; j < data[i].size(); j++) {
			f << "," << data[i][j];
		}
		f << endl;
	}
	f.close();
}

string Utils::calculateMD5(string file) {
	ifstream inFile;
	inFile.open (file, ios::binary | ios::in);
	
	inFile.seekg (0, ios::end);
	long length = inFile.tellg();
	inFile.seekg (0, ios::beg);
	
	char * fData = new char[length];
	inFile.read(fData, length);
	
	string ret = md5(fData, length);
	delete [] fData;
	
	return ret;
}

