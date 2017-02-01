#include "stdafx.h"
#include <stdio.h>
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <opencv2\highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\core\core.hpp>
#include <windows.h>
#include <stack>
#include <queue>

#define DYNAMIC_FEATURE_COUNT 8
#define STATIC_FEATURE_COUNT 5
#define FEATURE_COUNT DYNAMIC_FEATURE_COUNT+STATIC_FEATURE_COUNT
#define POSITION_COUNT 25
#define MODE 1

using namespace std;
using namespace cv;

enum JointType
{
	JointType_SpineBase = 0,
	JointType_SpineMid = 1,
	JointType_Neck = 2,
	JointType_Head = 3,
	JointType_ShoulderLeft = 4,
	JointType_ElbowLeft = 5,
	JointType_WristLeft = 6,
	JointType_HandLeft = 7,
	JointType_ShoulderRight = 8,
	JointType_ElbowRight = 9,
	JointType_WristRight = 10,
	JointType_HandRight = 11,
	JointType_HipLeft = 12,
	JointType_KneeLeft = 13,
	JointType_AnkleLeft = 14,
	JointType_FootLeft = 15,
	JointType_HipRight = 16,
	JointType_KneeRight = 17,
	JointType_AnkleRight = 18,
	JointType_FootRight = 19,
	JointType_SpineShoulder = 20,
	JointType_HandTipLeft = 21,
	JointType_ThumbLeft = 22,
	JointType_HandTipRight = 23,
	JointType_ThumbRight = 24,
};

int _tmain(int argc, _TCHAR* argv[])
{
	int i, j;
	const string input_position_filenames[POSITION_COUNT] = {
		"position_SpineBase.dat",
		"position_SpineMid.dat",
		"position_Neck.dat",
		"position_Head.dat",
		"position_ShoulderLeft.dat",
		"position_ElbowLeft.dat",
		"position_WristLeft.dat",
		"position_HandLeft.dat",
		"position_ShoulderRight.dat",
		"position_ElbowRight.dat",
		"position_WristRight.dat",
		"position_HandRight.dat",
		"position_HipLeft.dat",
		"position_KneeLeft.dat",
		"position_AnkleLeft.dat",
		"position_FootLeft.dat",
		"position_HipRight.dat",
		"position_KneeRight.dat",
		"position_AnkleRight.dat",
		"position_FootRight.dat",
		"position_SpineShoulder.dat",
		"position_HandTipLeft.dat",
		"position_ThumbLeft.dat",
		"position_HandTipRight.dat",
		"position_ThumbRight.dat"
	};

	string output_position_filenames[POSITION_COUNT] = {
		"position_SpineBase.dat",
		"position_SpineMid.dat",
		"position_Neck.dat",
		"position_Head.dat",
		"position_ShoulderLeft.dat",
		"position_ElbowLeft.dat",
		"position_WristLeft.dat",
		"position_HandLeft.dat",
		"position_ShoulderRight.dat",
		"position_ElbowRight.dat",
		"position_WristRight.dat",
		"position_HandRight.dat",
		"position_HipLeft.dat",
		"position_KneeLeft.dat",
		"position_AnkleLeft.dat",
		"position_FootLeft.dat",
		"position_HipRight.dat",
		"position_KneeRight.dat",
		"position_AnkleRight.dat",
		"position_FootRight.dat",
		"position_SpineShoulder.dat",
		"position_HandTipLeft.dat",
		"position_ThumbLeft.dat",
		"position_HandTipRight.dat",
		"position_ThumbRight.dat"
	};

	for (i = 0; i < POSITION_COUNT; i++){
		output_position_filenames[i] = "out_" + output_position_filenames[i];
	}

	vector<vector<Point3f>> connected_positions;
	for (i = 0; i < POSITION_COUNT; i++){
		vector<Point3f> connected_position;
		connected_positions.push_back(connected_position);
	}

	ofstream per_start_end_file("per_start_end.dat");
	queue<pair<int, int>> start_end_queue;
	if (MODE == 2){
		ifstream start_end_file("input_start_end.dat");
		string str;
		while (getline(start_end_file, str)){
			string tmp;
			istringstream stream(str);
			int c = 0;
			int key, value, per_start, per_end;
			while (getline(stream, tmp, ' ')){
				if (c == 0){
					key = stoi(tmp);
				}else if(c == 1){
					per_start = stoi(tmp);
				}else if(c == 2){
                    per_end = stoi(tmp);
				}else{
					value = stoi(tmp);
				}
				c++;
			}
			pair<int, int> p(key, value);
			start_end_queue.push(p);
			per_start_end_file << per_start-key << " " << per_end-key << endl; 
		}
		per_start_end_file.close();
		start_end_file.close();
	}

	ofstream start_end("start_end.dat");
	int freq = 0;
	while (true){
		int start, end;
		if (MODE == 1){
			cout << "start=";
			cin >> start;
			if (start == -1){
				break;
			}
			cout << "end=";
			cin >> end;
			start_end << start << " " << end << endl;
		}
		else{
			if (start_end_queue.empty()){
				break;
			}
			pair<int, int> p = start_end_queue.front();
			start = p.first;
			end = p.second;
			start_end_queue.pop();
		}

		if (start == 0 && end == 0){
			continue;
		}

		for (i = 0; i < POSITION_COUNT; i++){
			ifstream input_datafile;
			input_datafile.open(input_position_filenames[i]);
			if (input_datafile.fail()){
				cout << "Exception: ・ｽt・ｽ@・ｽC・ｽ・ｽ・ｽ・ｽ・ｽ・ｽ・ｽﾂゑｿｽ・ｽ・ｽ・ｽﾜゑｿｽ・ｽ・ｽ・ｽB" << endl;
				cin.get();
			}
			string str;
			vector<Point3f> positions;
			while (getline(input_datafile, str)){
				string tmp;
				istringstream stream(str);
				int c = 0;
				Point3f p;
				while (getline(stream, tmp, ' ')){
					float val = stof(tmp);
					if (c == 0){
						if (val < start || val > end){
							break;
						}
					}
					else if (c == 1){
						p.x = val;
					}
					else if (c == 2){
						p.y = val;
					}
					else{
						p.z = val;
						positions.push_back(p);
					}
					c++;
				}
			}
			string filename = to_string(freq) + "_" + output_position_filenames[i];
			ofstream output_file(filename);
			for (auto itr = positions.begin(); itr != positions.end(); ++itr){
				Point3f p = *itr;
				output_file << p.x << " " << p.y << " " << p.z << endl;
			}
			output_file.close();
		}
		freq++;
	}
	start_end.close();
	return 0;
}