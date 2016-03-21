# Introduction
MuMoSS (**Mu**lti**Mo**dal **S**cene **S**egmentator) is a video scene segmentator based on audiovisual features. The program features a set of different algorithms, using both the late and early fusion approaches.

# Requirements
*	A C++11 compatible compiler, like gcc
*	A functional OpenCV installation (version 2.4.x and above, SIFT suport is a must have!)
*   An appropriate set of CODECs (for video decoding)
*	Armadillo library v6.600+ (Avaliable at http://arma.sourceforge.net/)
*	libssl-dev (for its MD5 algorithm)

# Supported algorithms
## Simple Late Fusion (*slf*)
A simple late fusion based algorithm built upon Bag of Features dictionaries for both aural and visual features. After the dictionaries' are created with the specified size, a histogram are estimated for each keyframe/shot based on nearest neighbor method. The histograms are used to calculate the dissimilarity of adjacent shots and, if a similarity valley are found, a scene transition is detected.

## Simple Early Fusion (*sef*)
TODO

# Caveats
1. To data, the MD5 algorithm requires a lot of RAM.
2. This program may require a high amount of available memory. Typical values range from 2GB to 4GB for a 2h length 480p video.
2. The program' memory consumption is directly linked with the video length, resolution and number of keyframes. When facing memory problems, try to run with a smaller video with lower resolution or a fewer number of keyframes.
		


# Parameters
TODO

# Examples
TODO

# PROGRAM IN ACTIVE DEVELOPMENT
Avoid using until further notice...
