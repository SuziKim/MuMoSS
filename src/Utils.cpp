#include "Utils.hpp"

Mat Utils::ArmadilloKmeansClustering(vector<Mat> descriptors, int nClusters){
	int samples = 0;
	for(Mat d : descriptors) {
		samples = samples + d.rows;
	}
	
	/* Armadillo: "with each sample stored as a column vector" */
	arma::fmat data(descriptors[0].cols, samples);
	int col = 0;
	for(Mat d : descriptors) {
		for(int i = 0; i < d.rows; i++) {
			for(int j = 0; j < d.cols; j++) {
				data(j,col) = d.at<float>(i,j);
			}
			col++;
		}
	}
	arma::fmat result;
	arma::kmeans(result, data, nClusters, arma::random_subset, 250, true);
	
	/* Going back to cv::Mat format */	
	Mat ret(result.n_cols, result.n_rows, CV_32F);	
	for(int col = 0; col < result.n_cols; col++) {
		for(int row = 0; row < result.n_rows; row++) {
			ret.at<float>(col,row) = result(row,col);
		}
	}
	return ret;
}


Mat Utils::kmeansClustering(vector<Mat> descriptors, int nClusters){
	Mat samples, labels, dic;
	for(Mat m : descriptors) {
		samples.push_back(m);
	}
	kmeans(samples, nClusters, labels, TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 20, 0.1), 10, KMEANS_PP_CENTERS, dic);
	return dic;
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

double Utils::euclideanDistance(Mat d1, Mat d2) {
	double distance = 0.0;
	for(int i = 0; i < d1.cols; i++) {
		distance = distance + pow(d1.at<float>(i) - d2.at<float>(i),2);
	}	
	return sqrt(distance);
}

double Utils::euclideanDistance(vector<double> d1, vector<double> d2) {
	double distance = 0.0;
	for(int i = 0; i < d1.size(); i++) {
		distance = distance + pow(d1[i] - d2[i],2);
	}	
	return sqrt(distance);
}

void Utils::extractBoFHistogram(vector<double> &histogram, Mat &descriptor, Mat &dictionary) {
	/* Used when the keyframe does not contain any keypoint */
	if(descriptor.cols == 0 || descriptor.rows == 0) {
		for(int i = 0; i < dictionary.rows; i++) {
			histogram.push_back(1.0/dictionary.rows);
		}
		return;
	}
	if(descriptor.cols != dictionary.cols) {
		cout.clear();
		cout << "ERROR! The BoF dictionary and the descriptors should have the same size!" << endl;
		exit(1); 
	}

	for(int i = 0; i < dictionary.rows; i++) {
		histogram.push_back(0.0);
	}
	
	for(int i = 0; i < descriptor.rows; i++) {
		Mat desc = descriptor.row(i);
		vector<double> temp;
		for(int j = 0; j < dictionary.rows; j++) {
			Mat dic = dictionary.row(j);
			temp.push_back(Utils::euclideanDistance(desc,dic));
		}
		int indexMin = min_element(temp.begin(), temp.end()) - temp.begin();
		temp.clear();
		histogram[indexMin]++;		
	}
	
	double su = 0.0;
	for(int i = 0; i < histogram.size(); i++) {
		su = su + histogram[i];
	}
	for(int i = 0; i < histogram.size(); i++) {
		histogram[i] = histogram[i] / su;
	}
}

vector< vector< vector<double> > > Utils::histogramsConcat(vector< vector<double> > vHist, vector< vector<double> > aHist, vector< pair<int,int> > keyframes) {
	/* Each shot may have multiple descriptors */
	vector< vector< vector<double> > > shots(keyframes.back().first + 1);
	
	for(int i = 0; i < keyframes.size(); i++) {
		int shot = keyframes[i].first;
		
		vHist[i].insert(vHist[i].end(), aHist[shot].begin(), aHist[shot].end());
		shots[shot].push_back(vHist[i]);
	}
	
	return shots;
}

vector< vector< vector<double> > > Utils::generateShotsFromHistogram(vector< vector<double> > hist, vector< pair<int,int> > keyframes) {
	vector< vector< vector<double> > > shots(keyframes.back().first + 1);
	
	/* Its an aural histogram */
	if(hist.size() == shots.size()) {
		cout << "Aural histogram" << endl;
		for(int i = 0; i < hist.size(); i++) {
			shots[i].push_back(hist[i]);
		}
	} else {
		/* Its an visual histogram */
		cout << "Visual histogram" << endl;
		for(int i = 0; i < keyframes.size(); i++) {
			int shot = keyframes[i].first;		
			shots[shot].push_back(hist[i]);
		} 
	}
	
	return shots;
}

vector< pair<int,int> > Utils::sceneSegmentation(int windowsSize, double simFactor, vector< vector< vector<double> > > shots, vector< pair<int,int> > keyframes) {
	vector<double> similarity;
	similarity.push_back(0.0);
	
	for(int i = 1; i < shots.size(); i++) {		
		double minDistance = std::numeric_limits<double>::infinity();
		for(int j = i - windowsSize; j < i; j++) {
			if(j < 0) {
				continue;
			}
			minDistance = std::min(minDistance, Utils::minEuclideanDistance(shots[i], shots[j]));
		}	
		similarity.push_back(minDistance);
	}
	
	vector< pair<int,int> > scenes;
	int initial = 0;
	
	double realFactor = 1.0 - simFactor;
	for(int i = 1; i < similarity.size()-1; i++) {
		if(similarity[i-1]*realFactor >= similarity[i] && similarity[i+1]*realFactor >= similarity[i]) {
			scenes.push_back(make_pair(initial,i-1));
			initial = i;
		}
	}
	scenes.push_back(make_pair(initial,similarity.size()));
		
	return scenes;
}

double Utils::minEuclideanDistance(vector< vector<double> > h1, vector< vector<double> > h2) {
	double dist = std::numeric_limits<double>::infinity();
	
	for(int i = 0; i < h1.size(); i++) {
		for(int j = 0; j < h2.size(); j++) {
			dist = std::min(dist, Utils::euclideanDistance(h1[i],h2[j]));
		}
	}
	return dist;
}


vector< vector<double> > Utils::createHistogramsFromDescriptors(vector<Mat> descriptors, Mat dictionary) {
	vector< vector<double> > histograms(descriptors.size());
	vector<thread> pool;
	
	int index = 0;
	unsigned nThreads = thread::hardware_concurrency();
	for(int i = 0; i < descriptors.size(); i++) {
		if(pool.size() >= nThreads) {
			for(auto &t : pool) {
				t.join();
			}
			pool.clear();
		}
		pool.push_back(thread(&Utils::extractBoFHistogram, std::ref(histograms[i]), std::ref(descriptors[i]), std::ref(dictionary)));
	}
	
	for(auto &t : pool) {
		t.join();
	}
	
	pool.clear();
	return histograms;
}
