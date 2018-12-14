# About

This is sample project which does shadow detection on input video stream, also removes the detected shadows.

# Code Overview

The sample code usage is provided in Main.cpp
Sample code does following operations:
- Open video stream
- Read it frame by frame
- Learn background and get foreground by using OpenCV's Mixture of Gaussian method.
- Detect shadows by using cromacity method.
- Cleanup the detected shadows
- Remove detected shadows
- Add metadata to outputs
- Display output videos

Following are the utility classes:

- ChromacityShadowDet: Detects shadows using cromacity constancy method.
- CleanupShadows: Cleans shadows by object edge removal and removing noisy shadows.
- ShadowRemover: Removes detected shadows by adding difference between average background intensity to current pixel.
- ConfigParam: Contains parameters which needs to fine tuned as per input video.

* Depending on the input stream there is need to fine tune config parameters. Code can not work directly on all kind of input streams*

# Usage
- Run the executable.
- Give the path of input video stream
- It'll output 3 videos. Input stream, detected shadows marked in violet and shadow removed stream
- Press escape to exit.

# Sample Input and Output

Few samples input streams with corresponding outputs are kept in the folder 'Sample Input Output'
Input streams are taken from CAVIAR dataset.
Output streams are taken by doing screen capture of outputs.

*Prima facie code is giving more than 90% True Positive shadow detection output*
Need to further quantify this with ground truth data.

# Dependencies
Code is compiled in Visual Studio 2017 on Windows 10.
Uses OpenCV 3.4 pre-compiled libraries.

# Further Improvements
- Some false detection of shadows to be removed by using textural information.
- Code can be further optimized for speed.
- Performance evaluation by comparing ground truth data.

# References

- CAVIAR dataset: http://homepages.inf.ed.ac.uk/rbf/CAVIARDATA1/
- https://www.researchgate.net/publication/3193612_Detecting_Moving_Objects_Ghosts_and_Shadows_in_Video_Streams
- https://opencv.org/
