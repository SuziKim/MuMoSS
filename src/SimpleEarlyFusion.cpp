#include "SimpleEarlyFusion.hpp"

SimpleEarlyFusion::SimpleEarlyFusion(string videoPath, int vDicSize, int aDicSize, vector< pair<int,int> > keyframes, string aDescFolder, bool tempFiles) {
	this->videoPath = videoPath;
	this->vDicSize = vDicSize;
	this->aDicSize = aDicSize;
	this->keyframes = keyframes;
	this->aDescFolder = aDescFolder;
	this->tempFiles = tempFiles;
}

void SimpleEarlyFusion::execute() {
	string baseFile = "";
	if(this->tempFiles) {
		baseFile = InputOutput::calculateMD5(this->videoPath);
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
		InputOutput::checkFile(baseFile + "_sef_visual_histogram_" + to_string(this->vDicSize) + ".csv") &&
	 	InputOutput::checkFile(baseFile + "_sef_visual_dictionary_" + to_string(this->vDicSize) + ".csv")) {			 
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
	if(this->tempFiles && InputOutput::checkFile(baseFile + "_sef_visual_dictionary_" + to_string(this->vDicSize) + ".csv")) {
		cout << "Loading visual dictionary from file" << endl;
		visualDictionary = InputOutput::parseCSVDescriptor(baseFile + "_sef_visual_dictionary_" + to_string(this->vDicSize) + ".csv");
	} else {
		cout << "Creating visual dictionary" << endl;
		fVisualDic = std::async(&Utils::ArmadilloKmeansClustering, visualDescriptors, this->vDicSize);
		vDicControl = true;
	}	
	
	/* Aural Dictionary block */
	vector<Mat> auralDescriptors = InputOutput::parseAuralDescriptors(this->aDescFolder);	
	future<Mat> fAuralDic;	
	Mat auralDictionary;
	bool aDicControl = false;	
	if(this->tempFiles && InputOutput::checkFile(baseFile + "_sef_aural_dictionary_" + to_string(this->aDicSize) + ".csv")) {
		cout << "Loading aural dictionary from file" << endl;
		auralDictionary = InputOutput::parseCSVDescriptor(baseFile + "_sef_aural_dictionary_" + to_string(this->aDicSize) + ".csv");
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
			InputOutput::writeCSV(baseFile + "_sef_aural_dictionary_" + to_string(this->aDicSize) + ".csv", auralDictionary);
		}		
	}
		
	/* Aural Histogram block */
	future< vector< vector<double> > > fAuralHist;
	vector< vector<double> > auralHistogram;
	bool aHistControl = false;
	if(this->tempFiles && InputOutput::checkFile(baseFile + "_sef_aural_histogram_" + to_string(this->aDicSize) + ".csv")) {
		cout << "Loading aural histograms' from file" << endl;
		auralHistogram = InputOutput::parseCSVHistograms(baseFile + "_sef_aural_histogram_" + to_string(this->aDicSize) + ".csv");
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
			InputOutput::writeCSV(baseFile + "_sef_visual_dictionary_" + to_string(this->vDicSize) + ".csv", visualDictionary);
		}	
	}
	
	/* Aural Histogram saving block	*/
	if(aHistControl) {
		auralHistogram = fAuralHist.get();	
		cout << "Aural histogram successfully created" << endl;	
		if(this->tempFiles) {
			cout << "Salving aural histograms' into a file" << endl;
			InputOutput::writeCSV(baseFile + "_sef_aural_histogram_" + to_string(this->aDicSize) + ".csv", auralHistogram);
		}		
	}
	
	/* Visual Histogram block */
	future< vector< vector<double> > > fVisualHist;
	vector< vector<double> > visualHistogram;
	bool vHistControl = false;
	if(this->tempFiles && InputOutput::checkFile(baseFile + "_sef_visual_histogram_" + to_string(this->vDicSize) + ".csv")) {
		cout << "Loading visual histograms' from file" << endl;
		visualHistogram = InputOutput::parseCSVHistograms(baseFile + "_sef_visual_histogram_" + to_string(this->vDicSize) + ".csv");
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
			InputOutput::writeCSV(baseFile + "_sef_visual_histogram_" + to_string(this->vDicSize) + ".csv", visualHistogram);
		}
	}
	
	cout << "Performing multimodal early fusion" << endl;
	vector< vector< vector<double> > > shots = Utils::histogramsConcat(visualHistogram, auralHistogram, this->keyframes);
	
	cout << "Performing scene segmentation" << endl;
	/* TODO: Make the windowSize and simFactor as args */
	vector< pair<int,int> > scenes = Utils::sceneSegmentation(10, 0.2, shots, this->keyframes);
	/* The last scene should end at the last shot */
	scenes.back().second = this->keyframes.back().first;
	Utils::normalizePairs(scenes,1);
	
	
	cout << "Writing scene segmentation" << endl;
	string outFile = baseFile + "_sef_scene_segmentation_v" + to_string(this->vDicSize) + "_a" + to_string(this->aDicSize) + ".csv";
	if(InputOutput::checkFile(outFile)) {
		std::remove(outFile.c_str());
	}
	
	InputOutput::writeCSV(outFile, scenes);	
}


