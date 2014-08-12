//--------------------------------------------------------------------------------------------------
// Implementation of the paper "Exact Acceleration of Linear Object Detectors", 12th European
// Conference on Computer Vision, 2012.
//
// Copyright (c) 2012 Idiap Research Institute, <http://www.idiap.ch/>
// Written by Charles Dubout <charles.dubout@idiap.ch>
//
// This file is part of FFLD (the Fast Fourier Linear Detector)
//
// FFLD is free software: you can redistribute it and/or modify it under the terms of the GNU
// General Public License version 3 as published by the Free Software Foundation.
//
// FFLD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License along with FFLD. If not, see
// <http://www.gnu.org/licenses/>.
//--------------------------------------------------------------------------------------------------

#include "SimpleOpt.h"

#include "Intersector.h"
#include "Mixture.h"
#include "Scene.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "PedestrianDetector.h"




#include <sys/time.h>

using namespace FFLD;
using namespace std;


/*struct Detection : public FFLD::Rectangle
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
*/
///Main class
PedestrianDetector::PedestrianDetector()
{
}
PedestrianDetector::~PedestrianDetector()
{
}
void PedestrianDetector::start()
{
	
	//gettimeofday(&Start, 0);
}

int PedestrianDetector::stop()
{
	//gettimeofday(&Stop, 0);
	
	//timeval duration;
	//timersub(&Stop, &Start, &duration);
	
	return 0;//duration.tv_sec * 1000 + (duration.tv_usec + 500) / 1000;
}
void PedestrianDetector::test(int x)
{
}
void PedestrianDetector::draw(JPEGImage & image, const Rectangle & rect, uint8_t r, uint8_t g, uint8_t b, int linewidth)
{
	if (image.empty() || rect.empty() || (image.depth() < 3))
		return;
	
	const int width = image.width();
	const int height = image.height();
	const int depth = image.depth();
	uint8_t * bits = image.bits();
	
	// Draw 2 horizontal lines
	const int top = min(std::max(rect.top(), 1), height - linewidth - 1);
	const int bottom = min(std::max(rect.bottom(), 1), height - linewidth - 1);
	
	for (int x = std::max(rect.left() - 1, 0); x <= min(rect.right() + linewidth, width - 1); ++x) {
		if ((x != std::max(rect.left() - 1, 0)) && (x != min(rect.right() + linewidth, width - 1))) {
			for (int i = 0; i < linewidth; ++i) {
				bits[((top + i) * width + x) * depth    ] = r;
				bits[((top + i) * width + x) * depth + 1] = g;
				bits[((top + i) * width + x) * depth + 2] = b;
				bits[((bottom + i) * width + x) * depth    ] = r;
				bits[((bottom + i) * width + x) * depth + 1] = g;
				bits[((bottom + i) * width + x) * depth + 2] = b;
			}
		}
		
		// Draw a white line below and above the line
		if ((bits[((top - 1) * width + x) * depth    ] != 255) &&
			(bits[((top - 1) * width + x) * depth + 1] != 255) &&
			(bits[((top - 1) * width + x) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[((top - 1) * width + x) * depth + i] = 255;
		
		if ((bits[((top + linewidth) * width + x) * depth    ] != 255) &&
			(bits[((top + linewidth) * width + x) * depth + 1] != 255) &&
			(bits[((top + linewidth) * width + x) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[((top + linewidth) * width + x) * depth + i] = 255;
		
		if ((bits[((bottom - 1) * width + x) * depth    ] != 255) &&
			(bits[((bottom - 1) * width + x) * depth + 1] != 255) &&
			(bits[((bottom - 1) * width + x) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[((bottom - 1) * width + x) * depth + i] = 255;
		
		if ((bits[((bottom + linewidth) * width + x) * depth    ] != 255) &&
			(bits[((bottom + linewidth) * width + x) * depth + 1] != 255) &&
			(bits[((bottom + linewidth) * width + x) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[((bottom + linewidth) * width + x) * depth + i] = 255;
	}
	
	// Draw 2 vertical lines
	const int left = min(std::max(rect.left(), 1), width - linewidth - 1);
	const int right = min(std::max(rect.right(), 1), width - linewidth - 1);
	
	for (int y = std::max(rect.top() - 1, 0); y <= min(rect.bottom() + linewidth, height - 1); ++y) {
		if ((y != std::max(rect.top() - 1, 0)) && (y != min(rect.bottom() + linewidth, height - 1))) {
			for (int i = 0; i < linewidth; ++i) {
				bits[(y * width + left + i) * depth    ] = r;
				bits[(y * width + left + i) * depth + 1] = g;
				bits[(y * width + left + i) * depth + 2] = b;
				bits[(y * width + right + i) * depth    ] = r;
				bits[(y * width + right + i) * depth + 1] = g;
				bits[(y * width + right + i) * depth + 2] = b;
			}
		}
		
		// Draw a white line left and right the line
		if ((bits[(y * width + left - 1) * depth    ] != 255) &&
			(bits[(y * width + left - 1) * depth + 1] != 255) &&
			(bits[(y * width + left - 1) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[(y * width + left - 1) * depth + i] = 255;
		
		if ((bits[(y * width + left + linewidth) * depth    ] != 255) &&
			(bits[(y * width + left + linewidth) * depth + 1] != 255) &&
			(bits[(y * width + left + linewidth) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[(y * width + left + linewidth) * depth + i] = 255;
		
		if ((bits[(y * width + right - 1) * depth    ] != 255) &&
			(bits[(y * width + right - 1) * depth + 1] != 255) &&
			(bits[(y * width + right - 1) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[(y * width + right - 1) * depth + i] = 255;
		
		if ((bits[(y * width + right + linewidth) * depth    ] != 255) &&
			(bits[(y * width + right + linewidth) * depth + 1] != 255) &&
			(bits[(y * width + right + linewidth) * depth + 2] != 255))
			for (int i = 0; i < 3; ++i)
				bits[(y * width + right + linewidth) * depth + i] = 255;
	}
}
string PedestrianDetector::detect(const Mixture & mixture, int width, int height, const HOGPyramid & pyramid,
			double threshold, double overlap, const string & images, vector<Detection> & detections)
{
	// Compute the scores
	vector<HOGPyramid::Matrix> scores;
	vector<Mixture::Indices> argmaxes;
	vector<vector<vector<Model::Positions> > > positions;
	
	if (!images.empty())
		mixture.convolve(pyramid, scores, argmaxes, &positions);
	else
		mixture.convolve(pyramid, scores, argmaxes);
	
	// Cache the size of the models
	vector<std::pair<int, int> > sizes(mixture.models().size());
	
	for (int i = 0; i < sizes.size(); ++i)
		sizes[i] = mixture.models()[i].rootSize();
	
	// For each scale
	for (int i = pyramid.interval(); i < scores.size(); ++i) {
		// Scale = 8 / 2^(1 - i / interval)
		const double scale = pow(2.0, static_cast<double>(i) / pyramid.interval() + 2.0);
		
		const int rows = scores[i].rows();
		const int cols = scores[i].cols();
		
		for (int y = 0; y < rows; ++y) {
			for (int x = 0; x < cols; ++x) {
				const float score = scores[i](y, x);
				
				if (score > threshold) {
					if (((y == 0) || (x == 0) || (score > scores[i](y - 1, x - 1))) &&
						((y == 0) || (score > scores[i](y - 1, x))) &&
						((y == 0) || (x == cols - 1) || (score > scores[i](y - 1, x + 1))) &&
						((x == 0) || (score > scores[i](y, x - 1))) &&
						((x == cols - 1) || (score > scores[i](y, x + 1))) &&
						((y == rows - 1) || (x == 0) || (score > scores[i](y + 1, x - 1))) &&
						((y == rows - 1) || (score > scores[i](y + 1, x))) &&
						((y == rows - 1) || (x == cols - 1) || (score > scores[i](y + 1, x + 1)))) {
						Rectangle bndbox((x - pyramid.padx()) * scale + 0.5,
											   (y - pyramid.pady()) * scale + 0.5,
											   sizes[argmaxes[i](y, x)].second * scale + 0.5,
											   sizes[argmaxes[i](y, x)].first * scale + 0.5);
						
						// Truncate the object
						bndbox.setX(std::max(bndbox.x(), 0));
						bndbox.setY(std::max(bndbox.y(), 0));
						bndbox.setWidth(min(bndbox.width(), width - bndbox.x()));
						bndbox.setHeight(min(bndbox.height(), height - bndbox.y()));
						
						if (!bndbox.empty())
							detections.push_back(Detection(score, i, x, y, bndbox));
					}
				}
			}
		}
	}
	
	// Non maxima suppression
	std::sort(detections.begin(), detections.end());
	
	for (int i = 1; i < detections.size(); ++i)
		detections.resize(remove_if(detections.begin() + i, detections.end(),
									Intersector(detections[i - 1], overlap, true)) -
						  detections.begin());
	
	// Print the detection
	
	//const size_t lastDot = image.find_last_of('.');
	
	//string id = image.substr(0, lastDot);
	
	//const size_t lastSlash = id.find_last_of("/\\");

	
	//if (lastSlash != string::npos)
		//id = id.substr(lastSlash + 1);
	
	//if (out) {

		/*for (int i = 0; i < detections.size(); ++i)
			out  << ' ' << detections[i].score << ' ' << (detections[i].left() + 1) << ' '
				<< (detections[i].top() + 1) << ' ' << (detections[i].right() + 1) << ' '
				<< (detections[i].bottom() + 1) << ' ' << detections[i].width() << endl;*/
	//}

		
		//Take the closest person to detect

		if (detections.size()>0)
		{
			Detection detected = detections[0];
//#pragma omp critical
			for (int i = 0; i<detections.size();i++)
			{
					if (detections[i].width()>detected.width())
					{
						detected = detections[i];
					}
			}
		
		
			stringstream strsXTopLeft, strsYTopLeft,strsWidth,strsHeight;
		

			strsXTopLeft << detected.left();
			strsYTopLeft << detected.top();
			strsWidth << detected.width();
			strsHeight << detected.height();

			string temp_coords1 = strsXTopLeft.str() + ";" + strsYTopLeft.str() + ";" + strsWidth.str() + ";" + strsHeight.str();						
			return temp_coords1;
		}
		else 
		{
			string coords = "-1";
			return coords;
		}

	

}
	/*if (!images.empty()) {
		JPEGImage im(image);
		
		for (int j = 0; j < detections.size(); ++j) {
			// The position of the root one octave below
			const int argmax = argmaxes[detections[j].l](detections[j].y, detections[j].x);
			const int x2 = detections[j].x * 2 - pyramid.padx();
			const int y2 = detections[j].y * 2 - pyramid.pady();
			const int l = detections[j].l - pyramid.interval();
			
			// Scale = 8 / 2^(cd1 - j / interval)
			const double scale = pow(2.0, static_cast<double>(l) / pyramid.interval() + 2.0);
			
			for (int k = 0; k < positions[argmax].size(); ++k) {
				const Rectangle bndbox((positions[argmax][k][l](y2, x2)(0) - pyramid.padx()) *
											 scale + 0.5,
											 (positions[argmax][k][l](y2, x2)(1) - pyramid.pady()) *
											 scale + 0.5,
											 mixture.models()[argmax].partSize().second * scale + 0.5,
											 mixture.models()[argmax].partSize().second * scale + 0.5);
				
				draw(im, bndbox, 0, 0, 255, 2);
			}
			
			// Draw the root last
			//draw(im, detections[0], 255, 0, 0, 2);
		}
	
		draw(im, detections[0], 255, 0, 0, 2);
	im.save(images + '/' + id + ".jpg");
	}
return "h";*/

const char* PedestrianDetector::Run(int width, int height, int depth, const char* charbits,char* modelin, int padding, double threshold,int nbNegativeScenes, int interval, double overlap)
{
	// Default parameters
  //string image (image1);
	string model (modelin);
	Object::Name name = Object::PERSON;
	//string results("results.txt");
	string images(".");
	//int nbNegativeScenes = -1;
	//int padding = 12;
	//int interval = 10;
	//double threshold =-0.1;
	//double overlap = 0.5;

	

	// Try to open the mixture
	ifstream in(model.c_str(), ios::binary);
	if (!in.is_open()) {
		
		cerr << "\nInvalid model file " << model << endl;
		return "-1";
	}
	
	Mixture mixture;
	in >> mixture;
	
	if (mixture.empty()) {
		
		cerr << "\nInvalid model file " << model << endl;
		return "-1";
	}

	
	// The image/dataset
	//const string file(image);
	
	//const size_t lastDot = file.find_last_of('.');
	
	//if ((lastDot == string::npos) ||
	//	((file.substr(lastDot) != ".jpg") && (file.substr(lastDot) != ".txt"))) {
		
	//	cerr << "\nInvalid file " << file << ", should be .jpg or .txt" << endl;
	//	return "-1";
	//}
	
	// Try to open the results
	/*ofstream out;
	cout << "Results " << endl;
	if (!results.empty()) {
		out.open(results.c_str(), ios::binary);
		
		if (!out.is_open()) {
			
			cerr << "\nInvalid results file " << results << endl;
			return "-1";
		}
	}
*/
	// Try to load the image
	//if (file.substr(lastDot) == ".jpg") {
		//JPEGImage image(file);
		JPEGImage image(width,height,depth,(const uint8_t *) charbits);
		if (image.empty()) {
			
			cerr << "\nInvalid image " <<endl;
			return "-1";
		}
		
		// Compute the HOG features
		//start();

		
		HOGPyramid pyramid(image, padding, padding, interval);
		
		if (pyramid.empty()) {
			
			cerr << "\nInvalid image " <<endl;
			return "-1";
		}
		

		
		// Initialize the Patchwork class
		//start();

		if (!Patchwork::Init((pyramid.levels()[0].rows() - padding + 15) & ~15,
							 (pyramid.levels()[0].cols() - padding + 15) & ~15)) {
			cerr << "\nCould not initialize the Patchwork class" << endl;
			return "-1";
		}
		
		
		mixture.cacheFilters();
		

		
		// Compute the detections
		//start();
		
		vector<Detection> detections;

		string ctr = detect(mixture, image.width(), image.height(), pyramid, threshold, overlap, images, detections);
		char *coords = new char[ctr.length() + 1];
		strcpy(coords, ctr.c_str());

return coords;
	//}
/*	else { // ".txt"
		in.close();
		in.open(file.c_str(), ios::binary);
		
		if (!in.is_open()) {
			
			//cerr << "\nInvalid image_set file " << args.File(0) << endl;
			return -1;
		}
		
		// Find the annotations' folder (not sure that will work under Windows)
		size_t lastSlash = file.find_last_of("/\\");
		
		string folder;
		
		if (lastSlash != string::npos)
			folder = file.substr(0, lastSlash + 1);
		
		folder += "../../Annotations/";
		
		// Load all the scenes
		vector<Scene> scenes;
		
		int maxRows = 0;
		int maxCols = 0;
		
		while (in) {
			string line;
			getline(in, line);
			
			// Skip empty lines
			if (line.empty() || (line.size() < 3))
				continue;
			
			// A positive scene
			const bool positive = line.substr(line.size() - 2) == " 1";
			
			if (positive || nbNegativeScenes) {
				Scene scene(folder + line.substr(0, line.find(' ')) + ".xml");
				
				if (!scene.empty()) {
					scenes.push_back(scene);
				
					maxRows = std::max(maxRows, (scene.height() + 3) / 4 + padding);
					maxCols = std::max(maxCols, (scene.width() + 3) / 4 + padding);
					
					if (!positive)
						--nbNegativeScenes;
				}
			}
		}
		
		// Initialize the Patchwork class
		start();
		
		if (!Patchwork::Init((maxRows + 15) & ~15, (maxCols + 15) & ~15)) {
			cerr << "\nCould not initialize the Patchwork class" << endl;
			return -1;
		}
		
		cout << "Initialized FFTW in " << stop() << " ms" << endl;
		
		start();
		
		mixture.cacheFilters();
		
		cout << "Transformed the filters in " << stop() << " ms" << endl << "Testing "
#ifndef _WIN32
		//	 << scenes.size() << " scenes: \0337" << flush;
			 << scenes.size() << " scenes:" << flush;
#else
			 << scenes.size() << " scenes:" << flush;
#endif
		
		start();
		
		// The score of the detection associated to each object
		vector<vector<float> > detected(scenes.size());
		
		// Whether each object is difficult
		vector<vector<bool> > difficult(scenes.size());
		
		// The score of the detections associated to no object
		vector<float> negatives;
		
		int nbScenes = 0;
		
		// Most of the computations inside are already multi-threaded but the performance is higher
		// (~20% on my machine) if the threading is done at the level of the scenes rather than at a
		// lower level (pyramid levels/filters)
		// The performace measurements reported in the paper were done without this scene level
		// threading
		int i;
#pragma omp parallel for private(i)
		for (i = 0; i < scenes.size(); ++i) {
			JPEGImage image(scenes[i].filename());
			HOGPyramid pyramid(image, padding, padding, interval);
			
			// Compute the detections
			vector<Detection> detections;
			
			detect(mixture, image.width(), image.height(), pyramid, threshold, overlap,
				   scenes[i].filename(), out, images, detections);
			
			// Consider only objects of the right class
			for (int j = 0; j < scenes[i].objects().size(); ++j) {
				if (scenes[i].objects()[j].name() == name) {
					detected[i].push_back(-std::numeric_limits<float>::infinity());
					difficult[i].push_back(scenes[i].objects()[j].difficult());
				}
			}
			
#pragma omp critical
			{
				for (int j = 0; j < detections.size(); ++j) {
					// Find the most overlapped object with the same label
					Intersector intersector(detections[j]);
					double maxScore = 0.0;
					int object = -1;
					
					for (int k = 0, l = 0; k < scenes[i].objects().size(); ++k) {
						if (scenes[i].objects()[k].name() == name) {
							double score = 0.0;
							
							if (intersector(scenes[i].objects()[k].bndbox(), &score)) {
								if (score > maxScore) {
									maxScore = score;
									object = l;
								}
							}
							
							++l;
						}
					}
					
					if (object == -1) {
						negatives.push_back(detections[j].score);
						detections[j].score = -std::numeric_limits<float>::infinity();
					}
					else if (detections[j].score > detected[i][object]) {
						// Punish multiple detections
						if (detected[i][object] > -std::numeric_limits<float>::infinity())
							negatives.push_back(detected[i][object]);
						
						detected[i][object] = detections[j].score;
					}
					else if (detected[i][object] > -std::numeric_limits<float>::infinity()) {
						// Punish multiple detections
						negatives.push_back(detections[j].score);
					}
				}
				
				++nbScenes;
#ifndef _WIN32
			//	cout << "\0338" << fixed << setprecision(1) << (nbScenes * 100.0 / scenes.size())
				cout << ' ' << std::fixed << std::setprecision(1) << (nbScenes * 100.0 / scenes.size())
#else
				cout << ' ' << std::fixed << std::setprecision(1) << (nbScenes * 100.0 / scenes.size())
#endif
					 << "% (" << stop() << " ms)" << flush;
			}
		}
		
#ifndef _WIN32
	//	cout << "\0338100.0% (" << stop() << " ms)" << endl;
#endif
		
		// The score of the detections associated to objects
		vector<float> positives;
		
		int nbTotalPositives = 0;
		
		for (int i = 0; i < scenes.size(); ++i) {
			for (int j = 0; j < detected[i].size(); ++j) {
				if (!difficult[i][j]) {
					if (detected[i][j] > -std::numeric_limits<float>::infinity())
						positives.push_back(detected[i][j]);
					
					++nbTotalPositives;
				}
			}
		}
		
		// Sort the detections in decresing order
		std::sort(positives.begin(), positives.end(), std::greater<float>());
		std::sort(negatives.begin(), negatives.end(), std::greater<float>());
		
		positives.push_back(-std::numeric_limits<float>::infinity());
		negatives.push_back(-std::numeric_limits<float>::infinity());
		
		// Compute the Precision/Recall curve
		int nbTruePositives = 0;
		int nbFalsePositives = 0;
		
		vector<std::pair<double, double> > pr;
		
		for (int i = 0, j = 0; (i < positives.size() - 1) && (j < negatives.size() - 1);) {
			const float threshold = std::max(positives[i], negatives[j]);
			
			while (positives[i] >= threshold) {
				++nbTruePositives;
				++i;
			}
			
			while (negatives[j] >= threshold) {
				++nbFalsePositives;
				++j;
			}
			
			const double precision = static_cast<double>(nbTruePositives) /
									 (nbTruePositives + nbFalsePositives);
			const double recall = static_cast<double>(nbTruePositives) / nbTotalPositives;
			
			pr.push_back(std::make_pair(precision, recall));
		}
		
		// Add a point at 0 precision
		if (!pr.empty() && pr.back().first)
			pr.push_back(std::make_pair(0.0, pr.back().second));
		
		// Set the precision to the maximum among precision with at least this recall
		double previousPrecision = 0.0;
		
		for (int i = pr.size() - 1; i >= 0; --i) {
			pr[i].first = std::max(pr[i].first, previousPrecision);
			previousPrecision = pr[i].first;
		}
		
		// Compute the average precision
		cout << "Precision / Recall curve:" << endl;
		
		double averagePrecision = 0.0;
		double previousPrintedPrecision = 2.0;
		double previousPrintedRecall = -1.0;
		
		for (int i = 0; i < pr.size(); ++i) {
			if (((pr[i].first <= previousPrintedPrecision - 0.01) &&
				 ((i == pr.size() - 1) || (pr[i + 1].second != pr[i].second))) ||
				((pr[i].second >= previousPrintedRecall + 0.01) &&
				 ((i == pr.size() - 1) || (pr[i + 1].first != pr[i].first)))) {
				cout << std::setw(5) << std::fixed << std::setprecision(3) << pr[i].first << ' '
					 << std::setw(5) << std::fixed << std::setprecision(3) << pr[i].second << endl;
				previousPrintedPrecision = pr[i].first;
				previousPrintedRecall = pr[i].second;
			}
			
			if (i)
				averagePrecision += (pr[i].first + pr[i - 1].first) * 0.5 *
									(pr[i].second - pr[i - 1].second);
		}
		
		cout << "Average precision: " << std::setprecision(4) << averagePrecision << endl;
	}*/
	
	//return "hi";
}

int main()
{
	//PedestrianDetector pd;
	//pd.Run("../test.jpg");
	return 1;
}

