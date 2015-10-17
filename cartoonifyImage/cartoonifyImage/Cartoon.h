#include <stdlib.h>
#include <iostream>
#include <vector>

#include"opencv2/opencv.hpp"

using namespace cv ;
using namespace std ;

void cartoonifyImage(Mat srcColor , Mat dst , 
	bool sketchMode , bool alienMode , bool evilMode , bool debugMode);

void drawFaceStickFigure(Mat dst);

void changeFacialSkinColor(Mat smallImgBGR , Mat bigEdges , int debugType);
void removePepperNoise(Mat &mask);
void drawFaceStickFigure(Mat dst);