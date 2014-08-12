#include "TLD.h"
#include "ImAcq.h"
#include "Gui.h"

#ifndef MAINOTLD_H_
#define MAINOTLD_H_
class MainOTLD
{
	public:
    tld::TLD *tld;
    ImAcq *imAcq;
    tld::Gui *gui;
   
    bool showOutput;
    bool showTrajectory;
    int trajectoryLength;
    const char *printResults;
    const char *saveDir;
    double threshold;
    bool showForeground;
    bool showNotConfident;
    bool selectManually;
    int *initialBB;
    bool reinit;
    bool exportModelAfterRun;
    bool loadModel;
    const char *modelPath;
    const char *modelExportFile;
    int seed;
    MainOTLD(); 
    ~MainOTLD();
    void Init(int width, int height, const char* charbits);
    const char* Run(int width, int height, const char* newcharbits,int prevwidth, int prevheight, const char* oldcharbits);
    int Test(int width, int height, const char* charbits);
    void ToogleOptions(int option,int width, int height, const char* newcharbits);
    int Config();
    //int SelectNewBoundingBox(int width, int height, const char* charbits);
};

#endif /* MAIN_H_ */
