#include "InputOutput.hpp"

Mat InputOutput::parseCSVDescriptor(string filePath) {
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

/* Public */
bool InputOutput::checkFile(string name) {
  	if(FILE *file = fopen(name.c_str(),"r")) {
		fclose(file);
		return true;
	}
	return false;
}

bool InputOutput::checkFolder(string name) {
	struct stat info;	
	if(stat(name.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
		return true;
	} 
	return false;
}

vector< pair<int,int> > InputOutput::parseCSV(string filePath) {
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

vector<Mat> InputOutput::parseAuralDescriptors(string folder) {
	if(folder.back() != '/') {
		folder = folder + "/";
	}
	vector<Mat> ret;
	int base = 1;
	if(InputOutput::checkFile(folder + "audio_0")) {
		base = 0;
	}
	
	while(true) {
		string fTemp = folder + "audio_" + to_string(base);
		if(!InputOutput::checkFile(fTemp)) {
			break;
		}
		ret.push_back(InputOutput::parseCSVDescriptor(fTemp));		
		base++;
	}
	return ret;
}

void InputOutput::writeCSV(string file, Mat data) {
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

void InputOutput::writeCSV(string file, vector< vector<double> > data) {
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

void InputOutput::writeCSV(string file, vector< pair<int,int> > data) {
	ofstream f (file.c_str());
	f << data[0].first << "," << data[0].second;
	for(int i = 1; i < data.size(); i++) {
		f << endl << data[i].first << "," << data[i].second;
	}
	f.close();
}

vector< vector<double> > InputOutput::parseCSVHistograms(string filePath) {
	ifstream file(filePath.c_str());
	string line;
	string token;
	vector< vector<double> > values;
	
	if(file.is_open()) {
		while(getline(file,line)) {
			istringstream str(line);
			vector<double> temp;
			while(getline(str,token,',')) {
				temp.push_back(atof(token.c_str()));
			}
			values.push_back(temp);
			temp.clear();
		}
	}	
    	file.close();
    	
	return values;
}

string InputOutput::calculateMD5(string file) {
	
	unsigned char buffer[MD5_DIGEST_LENGTH];	
	FILE *f = fopen(file.c_str(),"rb");
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[4096];
	MD5_Init (&mdContext);
	while ((bytes = fread (data, 1, 4096, f)) != 0) {
		MD5_Update (&mdContext, data, bytes);
	}
	MD5_Final (buffer,&mdContext);
    fclose(f);
	
	stringstream ss;
	ss << std::hex << std::setfill('0');
	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)	{
		ss << std::setw(2) << static_cast<unsigned>(buffer[i]);
	}

	return ss.str();
}
