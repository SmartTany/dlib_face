#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <bitset>
#include <algorithm>
#include <vector>
#include <numeric>
//#include "yoloface.h"
#include "dlibface.h"

using namespace cv;
using namespace std;
using namespace dlib;                  


int main()
{
/*	YOLOv3::Detecting detectors;   	
    std::vector<YOLOv3::yoloresult> result;
	namedWindow("test");
        cv::Mat image = imread("../data/test.jpg");                            //opencv读取一个图片
        detectors.Detect(image, result);
        detectors.DrawResult(image, result);
        imshow("test", image);
        waitKey(0); 
        */
    FACE_RECOGNITION detectors;
    detectors.load_db_faces();

    while (1)
    {
        detectors.face_compares();
    }

}

