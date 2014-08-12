#include "SimpleOpt.h"

#include "Intersector.h"
#include "Mixture.h"
#include "Scene.h"




#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sys/time.h>
using namespace FFLD;
using namespace std;


struct Detection : public FFLD::Rectangle
{
	HOGPyramid::Scalar score;
	int l;
	int x;
	int y;
	
	Detection() : score(0), l(0), x(0), y(0)
	{
	}
	
	Detection(HOGPyramid::Scalar score, int l, int x, int y, FFLD::Rectangle bndbox) :
	FFLD::Rectangle(bndbox), score(score), l(l), x(x), y(y)
	{
	}
	
	bool operator<(const Detection & detection) const
	{
		return score > detection.score;
	}
};

typedef struct center
{
	int x,y;
	
} center;

class PedestrianDetector{
	public:
	//timeval Start, Stop;
	
	PedestrianDetector();
	~PedestrianDetector();
	void start();
	int stop();
	void test(int x);
	void draw(JPEGImage & image, const FFLD::Rectangle & rect, uint8_t r, uint8_t g, uint8_t b, int linewidth);
	string detect(const Mixture & mixture, int width, int height, const HOGPyramid & pyramid,double threshold, double overlap,const string & images, vector<Detection> & detections);
	const char* Run(int width, int height, int depth,const char* bits,char* modelin,  int padding, double threshold,int nbNegativeScenes, int interval, double overlap);
};

