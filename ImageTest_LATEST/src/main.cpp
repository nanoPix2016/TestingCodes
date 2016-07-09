/**
* Code to test the camera by changing the various parameters of the camera.
* Self-explanatory
* Author: ThreePixelsTeam
*/
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <raspicam/raspicam_cv.h>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace cv;

raspicam::RaspiCam_Cv Camera;
void dump_buffer(FILE *fp)
{
    int ch;
    int x=0;
    while(x<3)
    {

    ch=fgetc(fp);

    x++;
    cout<<x<<" "<<ch<<endl;
    }
    cout<<"exiting dump"<<endl;
}

struct cropParam
{
	int x = 0, y = 0, w = Camera.get(CV_CAP_PROP_FRAME_WIDTH), h = Camera.get(CV_CAP_PROP_FRAME_HEIGHT);
    cropParam() {}
    cropParam(int t1,int t2,int t3, int t4){
        x=t1; y=t2; w=t3; h=t4;
    }
}curCrop,inCrop;

struct multParam
{
	double b=1,g=1,r=1;
}curMult,inMult;

string displayCrop()
{
	stringstream s;
	s<<curCrop.x<<" "<<curCrop.y<<" "<<curCrop.w<<" "<<curCrop.h;
	return s.str();
}

string displayCropNorm()
{
	stringstream s;
	s<<(curCrop.x*1.0)/Camera.get(CV_CAP_PROP_FRAME_WIDTH)<<" "<<(curCrop.y*1.0)/Camera.get(CV_CAP_PROP_FRAME_HEIGHT)<<" "<<(curCrop.w*1.0)/Camera.get(CV_CAP_PROP_FRAME_WIDTH)<<" "<<(curCrop.h*1.0)/Camera.get(CV_CAP_PROP_FRAME_HEIGHT);
	return s.str();
}

string displayDim()
{
	stringstream s;
	s<<curCrop.w<<" "<<curCrop.h;
	return s.str();
}

string displayMult()
{
	stringstream s;
	s<<curMult.b<<" "<<curMult.g<<" "<<curMult.r;
	return s.str();
}

string displayRes()
{
	stringstream s;
	s<<Camera.get(CV_CAP_PROP_FRAME_WIDTH)<<" "<<Camera.get(CV_CAP_PROP_FRAME_HEIGHT);
	return s.str();
}

void doMult(Mat& fr,int layer,double mult)
{
	int i,j,maxi=curCrop.x+curCrop.w,maxj=curCrop.y+curCrop.h;
	for(i=curCrop.x;i<=maxi;i++)
		for(j=curCrop.y;j<=maxj;j++)
			fr.at<Vec3b>(j,i)[layer]=fr.at<Vec3b>(j,i)[layer]*mult;
}

void displayAll(raspicam::RaspiCam_Cv& Camera)
{
    cout<<"\nAll camera parameters:             "<<endl;
    cout<<"1 : Brightness(0-100):                 "<<Camera.get(CV_CAP_PROP_BRIGHTNESS)<<endl;
    cout<<"2 : ISO(0,100):                        "<<Camera.get(CV_CAP_PROP_GAIN)<<endl;
    cout<<"3 : Contrast(0-100):                   "<<Camera.get(CV_CAP_PROP_CONTRAST)<<endl;
    cout<<"4 : Saturation(0-100):                 "<<Camera.get(CV_CAP_PROP_SATURATION)<<endl;
    cout<<"5 : Shutter Speed(in ms):              "<<Camera.get(CV_CAP_PROP_EXPOSURE)<<endl;
    cout<<"6 : AWB Blue(0-100), 0 FOR AUTO:       "<<Camera.get(CV_CAP_PROP_WHITE_BALANCE_BLUE_U)<<endl;
    cout<<"7 : AWB Red(0-100, 0 FOR AUTO:         "<<Camera.get(CV_CAP_PROP_WHITE_BALANCE_RED_V)<<endl;
    cout<<"8 : Pixel Wise Crop(X,Y,W,H):          "<<displayCrop()<<endl;
    cout<<"9 : Normalised Crop(X,Y,W,H):          "<<displayCropNorm()<<endl;
    cout<<"10 : Width and Height:                 "<<displayDim()<<endl;
    cout<<"11 : BGR Multiplier(BGR)(Normalised):  "<<displayMult()<<endl;
    cout<<"12 : Resolution:                       "<<displayRes()<<endl;
    cout<<"13 : Sharpness(0-100):                 "<<Camera.get(CV_CAP_PROP_HUE)<<endl;
    cout<<"14 : Exit                              "<<endl;
}

void reload()
{
    ifstream ifile;
    if(ifstream("param.txt"))
    {
 	Camera.set(CV_CAP_PROP_FPS,30);
        ifile.open("param.txt");
        double a,b,c,d;
        ifile>>a>>b;
        Camera.set(CV_CAP_PROP_BRIGHTNESS,b);
        ifile>>a>>b;
        Camera.set(CV_CAP_PROP_GAIN,b);
        ifile>>a>>b;
        Camera.set(CV_CAP_PROP_CONTRAST,b);
        ifile>>a>>b;
        Camera.set(CV_CAP_PROP_SATURATION,b);
        ifile>>a>>b;
        if(b==-1)
        Camera.set(CV_CAP_PROP_EXPOSURE,0);
        else
        Camera.set(CV_CAP_PROP_EXPOSURE,b);
        ifile>>a>>b;
	 Camera.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U,b);
        ifile>>a>>b;
	 Camera.set(CV_CAP_PROP_WHITE_BALANCE_RED_V,b);
        ifile>>a>>curCrop.x>>curCrop.y>>curCrop.w>>curCrop.h;
        ifile>>a>>curMult.b>>curMult.g>>curMult.r;
        ifile>>a>>b>>c;
        Camera.set(CV_CAP_PROP_FRAME_WIDTH,b);
        Camera.set(CV_CAP_PROP_FRAME_HEIGHT,c);
        ifile>>a>>b;
        Camera.set(CV_CAP_PROP_HUE,b);
    }
}

int main () {
    int flag=0;
    Mat frame,fr;
    namedWindow("MyVideo",CV_WINDOW_NORMAL); //create a window called "MyVideo"
    namedWindow("MySnap",CV_WINDOW_AUTOSIZE);

    reload();
    cout<<"Opening Camera..."<<endl;
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    uint64_t pts,to=0;
    while(1){
             displayAll(Camera);
             while(1)
             {
                Camera.grab();
                Camera.retrieve ( frame,pts);
                printf("Presentation time stamp %lf milliseconds\n",(double)(pts-to)/1000.0  );
                to=pts;
		Rect myROI(curCrop.x,curCrop.y,curCrop.w,curCrop.h);
               frame = frame(myROI);
 		        vector<Mat> spl;
                vector<Mat> a2m;
                split(frame,spl);
                if(curMult.b < 1)
                    spl[0]*=curMult.b;		//doMult(frame, 0, curMult.b);
                if(curMult.g < 1)
                    spl[1]*=curMult.g;		//doMult(frame, 1, curMult.g);
                if(curMult.r < 1)
                    spl[2]*=curMult.r;		//doMult(frame, 2, curMult.r);

                a2m.push_back(spl[0]);
                a2m.push_back(spl[1]);
                a2m.push_back(spl[2]);
                Mat fr;
                merge(a2m,fr);

		
              /*  int num=waitKey(10);
                num&=255;

                if(num==48)
                {break;
                }
                else if(num==99)
                {
                imshow("MySnap",fr);
                waitKey(1);
                cout<<endl;
                cout<<endl;
                cout<<"Image file name(save as e.g 'foo.jpg' 'foo.png'"<<endl;
                string str;
                cin>>str;
                str="../savedImages/"+str;
				int flag=1;
                try{
					imwrite(str,fr);
				}
				catch(exception& ex) {
					cout<<"Sorry unable to save. Exception: "<<ex.what();
					flag=0;
				}
				if(flag)
					cout<<"Image saved successfully."<<endl;
                flag=0;
                }*/
               imshow("MyVideo", frame);
             // waitKey(1);

             }
             int pid;
             string line;
             cout<<endl;
             cout<<endl;

             cout<<"Please enter 'n' "<<endl;
             while(getline(cin,line))
             {
             if(line=="n")
             break;
             }
             int temp1,temp2;



             cout<<"Enter the prop id u want to change or 14 to exit e.g. 1 for brightness"<<endl;
             //cin.sync();
             cin>>pid;

             double value,x1,y1,w1,h1;
             int ww,hh;
             switch(pid)
             {
				case 8:
					cout<<"Enter 4 space separated values corresponding to (X,Y,W,H): "<<endl;
					cin>>inCrop.x>>inCrop.y>>inCrop.w>>inCrop.h;
					break;
				case 9:
					cout<<"Enter 4 space separated values normalised(0-1) corresponding to (X,Y,W,H): "<<endl;
					cin>>x1>>y1>>w1>>h1;
					break;
				case 10:
					cout<<"Enter the width and height values: "<<endl;
					cin>>ww>>hh;
					break;
				case 11:
					cout<<"Enter the normalised value of B, G and R in order: "<<endl;
					cin>>inMult.b>>inMult.g>>inMult.r;
					break;
				case 12:
					cout<<"Enter the width as multiples of 320. Height will be set accordingly: "<<endl;
					cin>>temp1;
					break;
                case 14:
                    break;
				default:
					if(pid>14)
						cout<<"\n\nWrong Choice. Try again.\n\n";
					else{
						cout<<"Enter the new value: "<<endl;
						cin>>value;
					}
			 }
             switch(pid)
             {
                case 1:
                    Camera.set(CV_CAP_PROP_BRIGHTNESS,value);
                    break;
                case 2:
                    Camera.set(CV_CAP_PROP_GAIN,value);
                    break;
                case 3:

                    Camera.set(CV_CAP_PROP_CONTRAST,value);
                    break;
                case 4:
                    Camera.set(CV_CAP_PROP_SATURATION,value);
                    break;
                case 5:

                    Camera.set(CV_CAP_PROP_EXPOSURE,value);
                    break;
                case 6:
                    Camera.set(CV_CAP_PROP_WHITE_BALANCE_BLUE_U,value);
                    break;
                case 7:
                    Camera.set(CV_CAP_PROP_WHITE_BALANCE_RED_V,value);
                    break;
                case 8:
					if((inCrop.x < 0)|(inCrop.y < 0)|(inCrop.w < 0)|(inCrop.h < 0)){
                        cout<<"\n\nNegative numbers not allowed. No changes commited. \nPlease try again.\n\n"<<endl;
                        break;
                    }
                    if(inCrop.x + inCrop.w > Camera.get(CV_CAP_PROP_FRAME_WIDTH))
                        inCrop.w = Camera.get(CV_CAP_PROP_FRAME_WIDTH) - inCrop.x;
                    if(inCrop.y + inCrop.h > Camera.get(CV_CAP_PROP_FRAME_HEIGHT))
                        inCrop.w = Camera.get(CV_CAP_PROP_FRAME_HEIGHT) - inCrop.y;
                    curCrop = inCrop;
                    break;
                case 9:
					if((x1 < 0)|(y1 < 0)|(w1 < 0)|(h1 < 0)) {
						cout<<"\n\nNegative numbers not allowed. No changes commited. \nPlease try again.\n\n"<<endl;
						break;
					}
					if((x1 > 1)|(y1 > 1)|(w1 > 1)|(h1 > 1)) {
						cout<<"\n\nNumbers should be lesser than 1. No changes commited. \nPlease try again.\n\n"<<endl;
						break;
					}
					if(x1 + w1 > 1)
						w1 = 1 - x1;
					if(y1 + h1 > 1)
						h1 = 1 - y1;
					curCrop.x = x1 * Camera.get(CV_CAP_PROP_FRAME_WIDTH);
					curCrop.y = y1 * Camera.get(CV_CAP_PROP_FRAME_HEIGHT);
					curCrop.w = w1 * Camera.get(CV_CAP_PROP_FRAME_WIDTH);
					curCrop.h = h1 * Camera.get(CV_CAP_PROP_FRAME_HEIGHT);
					break;
				case 10:
					if(ww > Camera.get(CV_CAP_PROP_FRAME_WIDTH))
						ww = Camera.get(CV_CAP_PROP_FRAME_WIDTH);
					if(hh > Camera.get(CV_CAP_PROP_FRAME_HEIGHT))
						hh = Camera.get(CV_CAP_PROP_FRAME_HEIGHT);
					temp1 = (Camera.get(CV_CAP_PROP_FRAME_WIDTH) - ww) / 2;
					temp2 = (Camera.get(CV_CAP_PROP_FRAME_HEIGHT) - hh) / 2;
					curCrop.x = temp1;
					curCrop.y = temp2;
					curCrop.w = ww;
					curCrop.h = hh;
					break;
				case 11:
					if((inMult.b < 0)|(inMult.g < 0)|(inMult.r < 0)) {
						cout<<"\n\nNegative numbers not allowed. No changes commited. \nPlease try again.\n\n"<<endl;
						break;
					}
					if((inMult.b > 1)|(inMult.g > 1)|(inMult.r > 1)) {
						cout<<"\n\nNumbers should not be greater than 1. No changes commited. \nPlease try again.\n\n"<<endl;
						break;
					}
					curMult = inMult;
					break;
				case 12:
					if((temp1%320)){
						cout<<"\n\nNot a multiple of 320. Please retry.\n\n"<<endl;
						break;
					}
					Camera.release();
					Camera.set(CV_CAP_PROP_FRAME_WIDTH,temp1);
					Camera.set(CV_CAP_PROP_FRAME_HEIGHT,temp1*3/4);
					if (!Camera.open()) {cerr<<"Error reopening the camera."<<endl;return -1;}
					curCrop = *(new cropParam(0,0,temp1,temp1*3/4));
					break;

                case 13:
                    Camera.set(CV_CAP_PROP_HUE,value);
                    break;

                case 14:
                    flag=1;
                    break;
             }
             if(flag)
             {
              flag=0;
              break;
             }
    }
    ofstream ofile;
    ofile.open("param.txt",ios::out|ios::trunc);
    ofile<<"1 "<<Camera.get(CV_CAP_PROP_BRIGHTNESS)<<endl;
    ofile<<"2 "<<Camera.get(CV_CAP_PROP_GAIN)<<endl;
    ofile<<"3 "<<Camera.get(CV_CAP_PROP_CONTRAST)<<endl;
    ofile<<"4 "<<Camera.get(CV_CAP_PROP_SATURATION)<<endl;
    ofile<<"5 "<<Camera.get(CV_CAP_PROP_EXPOSURE)<<endl;
    ofile<<"6 "<<Camera.get(CV_CAP_PROP_WHITE_BALANCE_BLUE_U)<<endl;
    ofile<<"7 "<<Camera.get(CV_CAP_PROP_WHITE_BALANCE_RED_V)<<endl;
    ofile<<"8 "<<displayCrop()<<endl;
    ofile<<"11 "<<displayMult()<<endl;
    ofile<<"12 "<<displayRes()<<endl;
    ofile<<"13 "<<Camera.get(CV_CAP_PROP_HUE)<<endl;
    ofile.close();
}
