#include "SimpleLateFusion.hpp"

SimpleLateFusion::SimpleLateFusion(string videoPath, int vDicSize, int aDicSize, vector< pair<int,int> > keyframes, string aDescFolder, bool tempFiles) {
	this->videoPath = videoPath;
	this->vDicSize = vDicSize;
	this->aDicSize = aDicSize;
	this->keyframes = keyframes;
	this->aDescFolder = aDescFolder;
	this->tempFiles = tempFiles;
}

void SimpleLateFusion::execute() {
	string baseFile = "";
	if(this->tempFiles) {
		baseFile = Utils::calculateMD5(this->videoPath);
		cout << "The video file MD5 is '" << baseFile << "'" << endl;
	}
	
	vector<int> desiredFrames;
	for(pair<int,int> kf : keyframes) {
		desiredFrames.push_back(kf.second);
	}
	
	unsigned nThreads = thread::hardware_concurrency();
	cout << "Running some tasks with " << to_string(nThreads) << " threads" << endl;
			
	cout << "Extracting visual SIFT descriptors" << endl;
	SiftExtractor sift(this->videoPath, nThreads, desiredFrames);
	sift.extract();	
	vector<Mat> visualDescriptors = sift.getDescriptors();
	
	/* Visual Dictionary block */
	Mat visualDictionary;
	future<Mat> fVisualDic;
	bool vDicControl = false;	
	if(this->tempFiles && Utils::checkFile(baseFile + "_slf_visual_dictionary_" + to_string(this->vDicSize) + ".csv")) {
		cout << "Loading visual dictionary from file" << endl;
		visualDictionary = Utils::parseCSVDescriptor(baseFile + "_slf_visual_dictionary_" + to_string(this->vDicSize) + ".csv");
	} else {
		cout << "Creating visual dictionary" << endl;
		fVisualDic = std::async(&Utils::ArmadilloKmeansClustering, visualDescriptors, this->vDicSize);
		vDicControl = true;
	}	
	
	/* Aural Dictionary block */
	vector<Mat> auralDescriptors = Utils::parseAuralDescriptors(this->aDescFolder);	
	future<Mat> fAuralDic;	
	Mat auralDictionary;
	bool aDicControl = false;	
	if(this->tempFiles && Utils::checkFile(baseFile + "_slf_aural_dictionary_" + to_string(this->aDicSize) + ".csv")) {
		cout << "Loading aural dictionary from file" << endl;
		auralDictionary = Utils::parseCSVDescriptor(baseFile + "_slf_aural_dictionary_" + to_string(this->aDicSize) + ".csv");
	} else {
		cout << "Creating aural dictionary" << endl;
		fAuralDic = std::async(&Utils::ArmadilloKmeansClustering, auralDescriptors, this->aDicSize);
		aDicControl = true;
	}
	
	/* Aural Dictionary saving block */
	if(aDicControl) {
		auralDictionary = fAuralDic.get();
		cout << "Aural dictionary successfully created" << endl;		
		if(this->tempFiles) {
			cout << "Salving aural dictionary into a file" << endl;
			Utils::writeCSVMat(baseFile + "_slf_aural_dictionary_" + to_string(this->aDicSize) + ".csv", auralDictionary);
		}		
	}
		
	/* Aural Histogram block */
	future< vector< vector<double> > > fAuralHist;
	vector< vector<double> > auralHistogram;
	bool aHistControl = false;
	if(this->tempFiles && Utils::checkFile(baseFile + "_slf_aural_histogram_" + to_string(this->aDicSize) + ".csv")) {
		cout << "Loading aural histograms' from file" << endl;
		auralHistogram = Utils::parseCSVHistograms(baseFile + "_slf_aural_histogram_" + to_string(this->aDicSize) + ".csv");
	} else {
		cout << "Creating aural histograms'" << endl;
		fAuralHist = std::async(&SimpleLateFusion::createHistograms, this, auralDescriptors, auralDictionary);
		aHistControl = true;
	}
		
	/* Visual Dictionary saving block */
	if(vDicControl) {	
		visualDictionary = fVisualDic.get();	
		cout << "Visual dictionary successfully created" << endl;	
		if(this->tempFiles) {
			cout << "Salving visual dictionary into a file" << endl;
			Utils::writeCSVMat(baseFile + "_slf_visual_dictionary_" + to_string(this->vDicSize) + ".csv", visualDictionary);
		}	
	}
	
	/* Aural Histogram saving block	*/
	if(aHistControl) {
		auralHistogram = fAuralHist.get();	
		cout << "Aural histogram successfully created" << endl;	
		if(this->tempFiles) {
			cout << "Salving aural histograms' into a file" << endl;
			Utils::writeCSVVector(baseFile + "_slf_aural_histogram_" + to_string(this->aDicSize) + ".csv", auralHistogram);
		}		
	}
	
	/* Visual Histogram block */
	future< vector< vector<double> > > fVisualHist;
	vector< vector<double> > visualHistogram;
	bool vHistControl = false;
	if(this->tempFiles && Utils::checkFile(baseFile + "_slf_visual_histogram_" + to_string(this->vDicSize) + ".csv")) {
		cout << "Loading visual histograms' from file" << endl;
		visualHistogram = Utils::parseCSVHistograms(baseFile + "_slf_visual_histogram_" + to_string(this->vDicSize) + ".csv");
	} else {
		cout << "Creating visual histograms'" << endl;
		fVisualHist = std::async(&SimpleLateFusion::createHistograms, this, visualDescriptors, visualDictionary);
		vHistControl = true;
	}
	
	/* Visual Histogram saving block */
	if(vHistControl) {
		visualHistogram = fVisualHist.get();	
		cout << "Visual histogram successfully created" << endl;	
		if(this->tempFiles) {
			cout << "Salving visual histograms' into a file" << endl;
			Utils::writeCSVVector(baseFile + "_slf_visual_histogram_" + to_string(this->vDicSize) + ".csv", visualHistogram);
		}
	}
	
	/* */
}

vector< vector<double> > SimpleLateFusion::createHistograms(vector<Mat> descriptors, Mat dictionary) {
	vector< vector<double> > histograms(descriptors.size());
	vector<thread> pool;
	
	int index = 0;
	unsigned nThreads = thread::hardware_concurrency();
	
	for(int i = 0; i < descriptors.size(); i++) {
		if(pool.size() >= nThreads) {
			for(int i = 0 ; i < pool.size(); i++) {
				pool[i].join();
			}
			pool.clear();
		}
		pool.push_back(thread(&Utils::extractBoFHistogram, std::ref(histograms[i]), std::ref(descriptors[i]), std::ref(dictionary)));
	}
	
	for(int i = 0 ; i < pool.size(); i++) {
		pool[i].join();
	}
	
	pool.clear();
	return histograms;
}


