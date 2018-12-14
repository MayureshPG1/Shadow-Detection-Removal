#include<opencv2/opencv.hpp>
#include"iostream"
#include "ChromacityShadowDet.h"
#include "ShadowRemover.h"
#include "CleanupShadows.h"
#include "ConfigParams.h"
using namespace std;
using namespace cv;

// to enable debugging mode
//#define DEBUG 1

int main()
{
	string path;
	cout << "Enter input video filepath with extension:";
	cin >> path;
	cout << "Press ESC to exit..." << endl;
	
	VideoCapture cap(path);
	// Check if camera opened successfully
	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	Mat bkgMat, frgMat, frgMask, accBkgMat, shadowMask, gradShadowMask;

	Ptr<BackgroundSubtractor> pBackSub;
	pBackSub = createBackgroundSubtractorMOG2(config::BKG_LEARNING_HISTORY_LENGTH, config::BKG_LEARNING_THRESHOLD, true);
	ChromacityShadowDet chromacityShadowDet;
	ShadowRemover shadowRemover;
	CleanupShadows cleanupShadow;
	Status returnVal = Status::SUCCESS;

	while (1)
	{
		Mat frame;
		// Capture frame-by-frame
		cap >> frame;

		// If the frame is empty, break immediately
		if (frame.empty())
			break;

		//to speed up
		resize(frame, frame, Size(config::FRAME_RESIZE_WIDTH, config::FRAME_RESIZE_HEIGHT));
		//remove noise
		GaussianBlur(frame, frame, Size(5, 5), 0.0);

		frgMat = Mat::zeros(frame.size(), frame.type());
		frgMask = Mat::zeros(frame.size(), frame.type());

		/*Learn backgroud using mixture of gaussian*/
		//update the background model
		pBackSub->apply(frame, frgMat, config::BKG_LEARNING_RATE);
		//get latest background image
		pBackSub->getBackgroundImage(bkgMat);

		/*cleanup foreground object*/
		//binarize detected foreground object
		threshold(frgMat, frgMat, 100, 255, CV_THRESH_BINARY);
		//do opening and closing on detected foreground
		int morph_size = 2;
		Mat element = getStructuringElement(MORPH_RECT, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
		morphologyEx(frgMat, frgMat, MORPH_CLOSE, element);
		morphologyEx(frgMat, frgMat, MORPH_OPEN, element);
		threshold(frgMat, frgMask, 200, 255, CV_THRESH_BINARY);

		/*get shadow based on chromaticity*/
		returnVal = chromacityShadowDet.GetShadowMask(frame, bkgMat, frgMask, shadowMask);
		if (returnVal == Status::FAILURE)
		{
			cout << "Chromacity Shadow detection Failed!";
			break;
		}


		/*cleanup shadows*/
		returnVal = cleanupShadow.RemoveObjectBoundries(frame, frgMask, shadowMask);
		if (returnVal == Status::FAILURE)
		{
			cout << "Cleanup Shadow by object boundries Failed!";
			break;
		}

		returnVal = cleanupShadow.RemoveNoisyObjects(shadowMask);
		if (returnVal == Status::FAILURE)
		{
			cout << "Cleanup boundries by noisy objects Failed!";
			break;
		}

		/*Remove shadows*/
		Mat shadowRemFrame = frame.clone();
		returnVal = shadowRemover.RemoveShadow(bkgMat, shadowMask, shadowRemFrame);
		if (returnVal == Status::FAILURE)
		{
			cout << "Shadow removal Failed!";
			break;
		}

		/*add metadata to output*/
		Mat opFrame = frame.clone();
		Scalar shadowColor(160, 0, 0);
		opFrame.setTo(shadowColor, shadowMask);
		//get the frame number and write it on the current frame
		stringstream ss;
		ss << cap.get(CAP_PROP_POS_FRAMES);
		string frameNumberString = "frame:" + ss.str();
		string shadowLegend = "Detected Shadow";
		putText(frame, frameNumberString.c_str(), cv::Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		putText(opFrame, frameNumberString.c_str(), cv::Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		putText(shadowRemFrame, frameNumberString.c_str(), cv::Point(15, 15), FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0));
		
		putText(opFrame, shadowLegend.c_str(), cv::Point(28, 30), FONT_HERSHEY_SIMPLEX, 0.5, shadowColor);
		circle(opFrame, Point(20, 25), 5, shadowColor,CV_FILLED);

		/*Display output*/
		imshow("Input Frame", frame);
		imshow("Shadow Detection", opFrame);
		imshow("Shadow Removal", shadowRemFrame);

#ifdef DEBUG
		imshow("frgMask", frgMask);
		imshow("bkgMat", bkgMat);
		imshow("shadowMask", shadowMask);
#endif // DEBUG

		// Press  ESC to exit
		char c = (char)waitKey(25);
		if (c == 27)
			break;
	}

	// When everything done, release the video capture object
	cap.release();

	// Closes all the frames
	destroyAllWindows();

	return 0;
}
