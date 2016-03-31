# Introduction
MuMoSS (**Mu**lti**Mo**dal **S**cene **S**egmentator) is a video scene segmentator based on audiovisual features. The program features a set of different algorithms, using both the late and early fusion approaches.

# Requirements
*	A C++11 compatible compiler, like gcc
*	A functional OpenCV installation (version 2.4.x and above, SIFT suport is a must have!)
*   An appropriate set of CODECs (for video decoding)
*	Armadillo library v6.600+ (Avaliable at http://arma.sourceforge.net/)
*	libssl-dev (for its MD5 algorithm)

# Usage
		./MuMoSS <videoFilePath> <keyframesFilePath> <auralDescriptorsFolder> <algorithm> <visualDictionarySize> <auralDictionarySize> [<useTemporaryFiles> <verbose>]
1. **videoFilePath** - The relative/absolute path to the video file which you want to segment.
2. **keyframesFilePath** - The relative/absolute path to the CSV file which describes the selected keyframes for each shot of the video. The first value must be the shot number and the second the (global) frame number. There needs to have at least one keyframe for each shot. If in doubt, check the FAST (https://github.com/Trojahn/FAST) program for more details.
3. **auralDescriptorsFolder** - A folder filled with CSV files of the descriptors for the audio stream. Each shot must use a single file, which can have multiple descriptors. The CSV files must have the names like *audio_0*, *audio_1* and so on. Every descriptor must have the same size.
4. **algorithm** - The desired scene segmentation algorithm. The description of the supported algorithms are presented below. Supported values: **slf**, **sef**.
5. **visualDictionarySize** - The number os descriptors in the visual dictionary. Must has a integer positive number.
6. **auralDictionarySize** - The number os descriptors in the aural dictionary. Must has a integer positive number.
7. (optional) **useTemporaryFiles** - A yes/no switch which enable/disable the creation of temporary files. Default: **yes**.
8. (optional) **verbose** - A yes/no switch which enable/disable the verbose mode of some infos regarding the currrent processing and other useful informations. Critical errors are printed regardless of this switch. Default: **yes**.


# Supported algorithms
## Simple Late Fusion (*slf*)
A late fusion based algorithm built upon Bag of Features dictionaries for both aural and visual features. After the dictionaries are created with the specified size, a histogram are estimated for each keyframe/shot based on nearest neighbor method. The histograms are used to calculate the dissimilarity of adjacent shots and, if a similarity valley are found, a scene transition is detected.

## Simple Early Fusion (*sef*)
This early fusion based algorithm is identical to the Simple Late Fusion (sfl) till the aural and visual features histograms are created. 
These histograms are now merged together using a concatenation method where the aural histogram is cloned to all the keyframes of each particular shot. 
After the merging procedure, the *multimodal histograms* are used to perform the scene segmentation (identical to those applied in the *slf* individual modality segmentation).

# Caveats
1. This program may require a high amount of available memory. Typical values range from 2GB to 4GB for a 2h length 480p video.
2. The program' memory consumption is directly related with the video length, resolution and number of keyframes. When facing memory problems, try to run with: a) a low resolution video; b) a video with loe duration; c) a fewer number of keyframes.
		
