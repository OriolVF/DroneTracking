/*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/**
  * @author Georg Nebehay
  */

#include "MainOTLD.h"
#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "TLDUtil.h"
#include "Trajectory.h"

using tld::Config;
using tld::Gui;
using tld::Settings;
using namespace tld;
using namespace cv;
using namespace std;

bool reuseFrameOnce;
bool skipProcessingOnce;
Trajectory trajectory;
//IplImage imInit,currimg;
IplImage *img;

MainOTLD::MainOTLD()
{
	//tld = new tld::TLD();
        //showOutput = 0;
        //printResults = NULL;
        //saveDir = ".";
        //threshold = 0.5;
        //showForeground = 0;

        //showTrajectory = false;
        //trajectoryLength = 0;

        //selectManually = 1;
	//showTrajectory = 1;
        initialBB = NULL;
        //showNotConfident = true;

        //reinit = 0;

       // loadModel = false;

       // exportModelAfterRun = false;
        //modelExportFile = "model";
       // seed = 0;

        gui = NULL;
       // modelPath = NULL;
}
MainOTLD::~MainOTLD()
{
	//delete tld;
        //imAcqFree(imAcq);
}
int MainOTLD::Test(int width, int height, const char* charbits)
{
	//printf("%d \n",width);	
	Mat image(height,width,CV_8UC3,(void*) charbits);
	//printf("%d \n",height);
	imshow("Test C++",image);
	return 99;
}

const char* MainOTLD::Run(int width, int height, const char* newcharbits, int prevwidth, int prevheight, const char* oldcharbits)
{
	double tic = cvGetTickCount();

   
    if(!reuseFrameOnce)
    {

		//cvReleaseImage(&img);
		Mat mimgnew(height,width,CV_8UC3,(void*) newcharbits);
		IplImage ipimg = mimgnew; 
    	img = &ipimg;	
	  	if(img == NULL)
		{
	   	 	printf("current image is NULL, assuming end of input.\n");
			return "-1";
		}
		
		}
		else
		{
			Mat mimg(prevheight,prevwidth,CV_8UC3,(void*) oldcharbits);  
			IplImage ipimg = mimg;     
			img = &ipimg;
				
		}
		Mat grey(img->height, img->width, CV_8UC1);
		cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);
		if(!skipProcessingOnce)
		{

		    tld->processImage(cvarrToMat(img));

		}
    else
    {
			
        skipProcessingOnce = false;
    }

    double toc = (cvGetTickCount() - tic) / cvGetTickFrequency();

    toc = toc / 1000000;

    float fps = 1 / toc;

    int confident = (tld->currConf >= threshold) ? 1 : 0;

    if(showOutput || saveDir != NULL)
    {
        char string[128];

        char learningString[10] = "";

        if(tld->learning)
        {
            strcpy(learningString, "Learning");
        }

        //sprintf(string, "#%d,Posterior %.2f; fps: %.2f, #numwindows:%d, %s", imAcq->currentFrame - 1, tld->currConf, fps, tld->detectorCascade->numWindows, learningString);
        CvScalar yellow = CV_RGB(255, 255, 0);
        CvScalar blue = CV_RGB(0, 0, 255);
        CvScalar black = CV_RGB(0, 0, 0);
        CvScalar white = CV_RGB(255, 255, 255);

    if(tld->currBB != NULL)
    {
        CvScalar rectangleColor = (confident) ? blue : yellow;
        cvRectangle(img, tld->currBB->tl(), tld->currBB->br(), rectangleColor, 8, 8, 0);
	

		if(showTrajectory)
		{
			CvPoint center = cvPoint(tld->currBB->x+tld->currBB->width/2, tld->currBB->y+tld->currBB->height/2);
			cvLine(img, cvPoint(center.x-2, center.y-2), cvPoint(center.x+2, center.y+2), rectangleColor, 2);
			cvLine(img, cvPoint(center.x-2, center.y+2), cvPoint(center.x+2, center.y-2), rectangleColor, 2);
			trajectory.addPoint(center, rectangleColor);
		}	
		
            }
	    else if(showTrajectory)
	    {
		trajectory.addPoint(cvPoint(-1, -1), cvScalar(-1, -1, -1));
            }
	    if(showTrajectory)
	    {
		trajectory.drawTrajectory(img);
	    }

           // CvFont font;
            //cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, .5, .5, 0, 1, 8);
            //cvRectangle(img, cvPoint(0, 0), cvPoint(img->width, 50), black, CV_FILLED, 8, 0);
            //cvPutText(img, string, cvPoint(25, 25), &font, white);

            if(showForeground)
            {

                for(size_t i = 0; i < tld->detectorCascade->detectionResult->fgList->size(); i++)
                {
                    Rect r = tld->detectorCascade->detectionResult->fgList->at(i);
                }

            }

            if(showOutput)
            {

                gui->showImage(img);
                char key = gui->getKey();

                //if(key == 'q') break;

                if(key == 'b')
                {

                    ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;

                    if(fg->bgImg.empty())
                    {
                        fg->bgImg = grey.clone();
                    }
                    else
                    {
                        fg->bgImg.release();
                    }
                }

                if(key == 'c')
                {
                    //clear everything
                    tld->release();
                }

                if(key == 'l')
                {
                    tld->learningEnabled = !tld->learningEnabled;
                    printf("LearningEnabled: %d\n", tld->learningEnabled);
                }

                if(key == 'a')
                {
                    tld->alternating = !tld->alternating;
                    printf("alternating: %d\n", tld->alternating);
                }

                if(key == 'e')
                {
                    tld->writeToFile(modelExportFile);
                }

                if(key == 'i')
                {
                    tld->readFromFile(modelPath);
                }

                if(key == 'r')
                {
                    CvRect box;

                    if(getBBFromUser(img, box, gui) == PROGRAM_EXIT)
                    {
                        return "-1";
                    }

                    Rect r = Rect(box);

                    tld->selectObject(grey, &r);
                }
            }

            if(saveDir != NULL)
            {

                char fileName[256];
              //  sprintf(fileName, "%s/%.5d.png", saveDir, imAcq->currentFrame - 1);

                //cvSaveImage(fileName, img);
            }
        }

        if(reuseFrameOnce)
        {
            reuseFrameOnce = false;
        }

	 //cvReleaseImage(&img);

    	img = NULL;

	
	if(exportModelAfterRun)
        {
          tld->writeToFile(modelExportFile);
        }
	Rect *bb;
	if (tld->currBB == NULL) 
	{
	 	 bb = tld->prevBB;
	}
	else
	{
		bb = tld->currBB;
	}
	
	string Stringboundingbox; 
	if(confident)
	{
		stringstream sstlX,sstlY,ssbbWidth,ssbbHeight;

		sstlX << bb->x;
		sstlY << bb->y;
		ssbbWidth << bb->width;
		ssbbHeight << bb->height;	
		
		Stringboundingbox = sstlX.str() + ";" + sstlY.str() + ";" + ssbbWidth.str() + ";" + ssbbHeight.str();
	}
	else
	{
		Stringboundingbox= "-1";				
	}

	char *boundingBox = new char[Stringboundingbox.length()+1];
	strcpy(boundingBox,Stringboundingbox.c_str());	

return boundingBox;
	
}

void MainOTLD::ToogleOptions(int option, int width, int height, const char* newcharbits)
{
	switch (option)
	{
		case 0: 
			
			tld->learningEnabled = !tld->learningEnabled;
        		printf("LearningEnabled: %d\n", tld->learningEnabled);
			break;

		case 1:  tld->alternating = !tld->alternating;
                    	 printf("alternating: %d\n", tld->alternating);
			 break;
		
		/*case 2:  
               Mat mimgnew(height,width,CV_8UC3,(void*) newcharbits);
			   IplImage ipimg = mimgnew; 
        	   img = &ipimg;
			   Mat grey(img->height, img->width, CV_8UC1);
			   cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);
			   ForegroundDetector *fg = tld->detectorCascade->foregroundDetector;

	           if(fg->bgImg.empty())
	            {
	                fg->bgImg = grey.clone();
					printf("Enable background\n");
	            }
	            else
	            {
	                fg->bgImg.release();
		        	printf("Disable background\n");
	            }
			  
			   break;	*/
		
		case 2: 
					
 					Mat mimgnew(height,width,CV_8UC3,(void*) newcharbits);
			   		IplImage ipimg = mimgnew; 
        	        img = &ipimg;
			   		Mat grey(img->height, img->width, CV_8UC1);
			   		cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);
					gui->showImage(img);
					CvRect box;

                    if(getBBFromUser(img, box, gui) == PROGRAM_EXIT)
                    {
                        return;
                    }
					else
					{
                    	Rect r = Rect(box);
                    	tld->selectObject(grey, &r);
					}
				break;
	}
	
}

/*int MainOTLD::SelectNewBoundingBox(int width, int height, const char* charbits)
{
	CvRect box;

	if(getBBFromUser(img, box, gui) == PROGRAM_EXIT)
	{
		return -1;
	}

	Rect r = Rect(box);
	Mat mimg(height,width,CV_8UC3,(void*) charbits);  
	IplImage ipimg = mimg;     
	img = &ipimg;
				

	Mat grey(img->height, img->width, CV_8UC1);
	cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);
	tld->selectObject(grey, &r);
	return 1;
	
}*/
void MainOTLD::Init(int width, int height, const char* charbits)
{

 ////
	
	//Config config;
	gui = new Gui();
	
	gui->init();
	//imAcq = imAcqAlloc();
	//imAcq->method = IMACQ_STREAM;
	
	tld = new tld::TLD();
	Config();
	srand(seed);
	
	Mat mimg(height,width,CV_8UC3,(void*) charbits); 
	IplImage ipimg = mimg;      
	img = &ipimg;
 
    	Mat grey(img->height, img->width, CV_8UC1);
    	cvtColor(cvarrToMat(img), grey, CV_BGR2GRAY);

    	tld->detectorCascade->imgWidth = grey.cols;
    	tld->detectorCascade->imgHeight = grey.rows;
    	tld->detectorCascade->imgWidthStep = grey.step;

	if(showTrajectory)
	{
		trajectory.init(trajectoryLength);
	}

    if(selectManually)
    {
	
        CvRect box;

        if(getBBFromUser(img, box, gui) == PROGRAM_EXIT)
        {
            return;
        }

        if(initialBB == NULL)
        {
            initialBB = new int[4];
        }

        initialBB[0] = box.x;
        initialBB[1] = box.y;
        initialBB[2] = box.width;
        initialBB[3] = box.height;
    }

    FILE *resultsFile = NULL;

    if(printResults != NULL)
    {
        resultsFile = fopen(printResults, "w");
        if(!resultsFile)
        {
            fprintf(stderr, "Error: Unable to create results-file \"%s\"\n", printResults);
            exit(-1);
        }
    }

    reuseFrameOnce = false;
    skipProcessingOnce = false;

    if(loadModel && modelPath != NULL)
    {
        tld->readFromFile(modelPath);
        reuseFrameOnce = true;
    }
    else if(initialBB != NULL)
    {
        Rect bb = tldArrayToRect(initialBB);

        printf("Starting at %d %d %d %d\n", bb.x, bb.y, bb.width, bb.height);

        tld->selectObject(grey, &bb);
        skipProcessingOnce = true;
        reuseFrameOnce = true;
    } 

    cvDestroyAllWindows();

  
}

int MainOTLD::Config()
{
    /*ImAcq *ImAcqAux = imAcq;

    // imAcq
    ImAcqAux->method = m_settings.m_method;
    ImAcqAux->imgPath = (m_settings.m_imagePath.empty()) ? NULL : m_settings.m_imagePath.c_str();
    ImAcqAux->lastFrame = m_settings.m_lastFrame;
    ImAcqAux->currentFrame = m_settings.m_startFrame;
    ImAcqAux->camNo = m_settings.m_camNo;
    ImAcqAux->fps = m_settings.m_fps;*/

    // main
    tld->trackerEnabled = true;
    showOutput = false;
    showTrajectory =  false;
    trajectoryLength = true;
    printResults = false;
    saveDir =  NULL;
    threshold = 0.1;
    showForeground = false;
    showNotConfident = true;
    tld->alternating = false;
    tld->learningEnabled = true;
    selectManually = true;
    exportModelAfterRun = false;
    modelExportFile = "model";
    loadModel = false;
    modelPath = NULL;
    seed = 0;

    /*if(m_settings.m_initialBoundingBox.size() > 0)
    {
        main->initialBB = new int[4];

        for(int i = 0; i < 4; i++)
        {
            main->initialBB[i] = m_settings.m_initialBoundingBox[i];
        }
    }*/

    //DetectorCascade *detectorCascade = main->tld->detectorCascade;
    tld->detectorCascade->varianceFilter->enabled = true;
    tld->detectorCascade->ensembleClassifier->enabled = true;
    tld->detectorCascade->nnClassifier->enabled = true;

    // classifier
    tld->detectorCascade->useShift = true;
    tld->detectorCascade->shift = 0.1;
    tld->detectorCascade->minScale = -10;
    tld->detectorCascade->maxScale = 10;
    tld->detectorCascade->minSize = 25;
    tld->detectorCascade->numTrees = 10;
    tld->detectorCascade->numFeatures = 10;
    tld->detectorCascade->nnClassifier->thetaTP =0.65;
    tld->detectorCascade->nnClassifier->thetaFP = 0.5;

    return SUCCESS;
}
int main(int argc, char **argv)
{
	return 1;
}
