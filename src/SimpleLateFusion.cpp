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
			
			
	vector<Mat> visualDescriptors;	
	/* If there is a visual dictionary and histogram, there is't the need to extract the visual descriptors! */
	if(this->tempFiles &&
		Utils::checkFile(baseFile + "_slf_visual_histogram_" + to_string(this->vDicSize) + ".csv") &&
	 	Utils::checkFile(baseFile + "_slf_visual_dictionary_" + to_string(this->vDicSize) + ".csv")) {			 
		cout << "Visual dictionary and histograms found. Skipping visual descriptors extraction." << endl;
	} else {
		cout << "Extracting visual SIFT descriptors" << endl;
		SiftExtractor sift(this->videoPath, nThreads, desiredFrames);
		sift.extract();	
		visualDescriptors = sift.getDescriptors();	
	}
		
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
		fAuralHist = std::async(&Utils::createHistogramsFromDescriptors, auralDescriptors, auralDictionary);
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
		fVisualHist = std::async(&Utils::createHistogramsFromDescriptors, visualDescriptors, visualDictionary);
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
	
	cout << "Performing visual scene segmentation" << endl;
	/* TODO: Make the windowSize and simFactor as args */
	vector< vector< vector<double> > > visualShots = Utils::generateShotsFromHistogram(visualHistogram, this->keyframes);
	vector< pair<int,int> > visualScenes = Utils::sceneSegmentation(10, 0.2, visualShots, this->keyframes);
	
	cout << "Perfoming aural scene segmentation" << endl;
	vector< vector< vector<double> > > auralShots = Utils::generateShotsFromHistogram(auralHistogram, this->keyframes);
	vector< pair<int,int> > auralScenes = Utils::sceneSegmentation(10, 0.2, auralShots, this->keyframes);
	
	cout << "Perfoming multimodal late fusion" << endl;	
	vector< pair<int,int> > scenes = this->scenesFusion(visualScenes, auralScenes);
	Utils::normalizePairs(scenes,1);
	
	cout << "Writing scene segmentation" << endl;
	string outFile = baseFile + "_slf_scene_segmentation_v" + to_string(this->vDicSize) + "_a" + to_string(this->aDicSize) + ".csv";
	if(Utils::checkFile(outFile)) {
		std::remove(outFile.c_str());
	}
	
	Utils::writeOutputFile(outFile, scenes);	
}


vector< pair<int,int> > SimpleLateFusion::scenesFusion(vector< pair<int,int> > visualScenes, vector< pair<int,int> > auralScenes) {
	vector<bool> vTrans(visualScenes.back().second+1, false);
	vector<bool> aTrans(auralScenes.back().second+1, false);
	vector<bool> fTrans(visualScenes.back().second+1, false);
	
	for(pair<int,int> p : visualScenes) {
		vTrans[p.second] = true;
	}
	vTrans.back() = false;
	
	for(pair<int,int> p : auralScenes) {
		aTrans[p.second] = true;
	}
	aTrans.back() = false;
	
	/* If the two modalities says that there is a transition, accept it */
	for(int i = 0; i < vTrans.size(); i++) {
		if(vTrans[i] && aTrans[i]) {
			fTrans[i] = true;
			/* Remove those transitions from further analysis */
			vTrans[i] = false;
			aTrans[i] = false;
		}		
	}
		
	/* If there is two consecutive transitions, accept the first one */
	for(int i = 1; i < vTrans.size()-1; i++) {
		if(vTrans[i] && vTrans[i+1]) {
			fTrans[i] = true;
			vTrans[i] = false;
			vTrans[i+1] = false;
		}
		if(aTrans[i] && aTrans[i+1]) {
			fTrans[i] = true;
			aTrans[i] = false;
			aTrans[i+1] = false;
		}
		if((aTrans[i] && vTrans[i+1]) || (vTrans[i] && aTrans[i+1])) {
			fTrans[i] = true;
			aTrans[i] = false;
			aTrans[i+1] = false;
			vTrans[i] = false;
			vTrans[i+1] = false;
		}		
	}
	
	/* TODO: Variable windows size? */
	int acceptWindow = 3;
	for(int i = 0; i < vTrans.size(); i++) {
		
		if(vTrans[i] || aTrans[i]) {
			int maxIndex = i;
			int minIndex = i;
			
			for(int j = i - acceptWindow; j < i; j++) {
				if(j < 0) {
					continue;
				}
				if(aTrans[j] || vTrans[j]) {
					minIndex = j;
					break;
				}
			}
			
			for(int j = i + acceptWindow; j > i; j--) {
				if(j > vTrans.size()) {
					continue;
				}
				if(aTrans[j] || vTrans[j]) {
					minIndex = j;
					break;
				}
			}
			
			for(int j = i - acceptWindow; j <= i; j++) {
				if(j < 0) {
					continue;
				}
				aTrans[j] = false;
				vTrans[j] = false;
			}
			
			for(int j = i + acceptWindow; j >= i; j--) {
				if(j > vTrans.size()) {
					continue;
				}
				aTrans[j] = false;
				vTrans[j] = false;
			}
			
			
			int avgIndex = (maxIndex + minIndex) / 2;
			fTrans[avgIndex] = true;
		}		
	}
	
	/* Convert back from vector<bool> to vector<pair<int,int>> */
	
	int initial = 0;
	vector< pair<int,int> > ret;
	for(int i = 1; i < fTrans.size(); i++) {
		if(fTrans[i]) {
			ret.push_back(make_pair(initial,i));
			initial = i+1;
		}
	}
	ret.push_back(make_pair(initial, fTrans.size()));
	
	return ret;	
}


