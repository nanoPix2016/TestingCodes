/*
    Constant video capture runs and saves when triggered
*/
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <raspicam/raspicam_cv.h>
#include "opencv2/imgproc/imgproc_c.h"
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/opencv.hpp>
#include <sys/time.h>
using namespace std;
using namespace cv;

#define	PIN1 0
#define GAP 8500

static volatile int globalcounter1 = 0 ;

void myInterrupt1(void)
{
//cout<<"called"<<endl;
  ++globalcounter1;

}

inline int aabs(uint64_t t1,uint64_t t2)
{
    int ans=(t1-t2);
    if (ans>=0)
        return ans;
    else
        return (-1)*ans;
}

int main ( int argc,char **argv )
{
    string str = (argv[1]);
    cout<<"Currently on file: "<<str<<endl;
    str="data_"+str;
    str=str+".txt";
    const char* ch=&str[0];
    freopen(ch,"w",stdout);
    wiringPiSetup() ;
    pinMode (PIN1, INPUT) ;
    int myc1 = 0 , lc1 = 0 ;
    if(wiringPiISR(PIN1, INT_EDGE_FALLING, &myInterrupt1) < 0)
    {
         fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
         return 1 ;
    }
    raspicam::RaspiCam_Cv Camera;
    cv::Mat frame;
    frame.create ( Camera.get(CV_CAP_PROP_FRAME_HEIGHT), Camera.get(CV_CAP_PROP_FRAME_WIDTH), Camera.get(CV_CAP_PROP_FORMAT) );
    namedWindow("MyVideo",CV_WINDOW_NORMAL); //create a window called "MyVideo"
    namedWindow("Mysnap",CV_WINDOW_NORMAL);
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
uint64_t t0,time;
   Mat fr; Mat fr1,fr2;
   uint64_t to;
   uint64_t pts;

//uncomment while using GPIO trigger

   int cntr=0,err=0,ct100=0;
   while(1){


       while(myc1==globalcounter1)
       {

       }
       cntr++;
       to=Camera.getTime();
     //printf("Trigger Time: %lf millisecond\n", (double)to/1000.0);
        Camera.grab(frame,pts);
       //printf("Presentation time stamp %lf milliseconds\n",(double)(pts)/1000.0  );
    if((aabs(to,pts)>GAP)&&(pts<to)){
        //printf("Skipped 1 : Difference is %d\n",aabs(to,pts)) ;
        Camera.grab(frame,pts);
    }
    //printf("Presentation time stamp %lf milliseconds\n",(double)(pts)/1000.0  );

    if((aabs(to,pts)>GAP)&&(pts<to)){
        //printf("Skipped 2 : Difference is %d\n",aabs(to,pts)) ;
        Camera.grab(frame,pts);
    }

    if((aabs(to,pts)>GAP)&&(pts<to)){
        //printf("Skipped 3 : Difference is %d\n",aabs(to,pts)) ;
        Camera.grab(frame,pts);
    }
      //twr=Camera.getTime();
        //stringstream ss;
        //ss<<ct100*100+cntr<<".jpg";
        //str=ss.str();
        //imwrite(str,frame);
      //  printf("Time taken to write %lf milliseconds\n",(double)(Camera.getTime()-twr)/1000.0  );*

    //printf("Presentation time stamp %lf milliseconds\n",(double)(pts)/1000.0  );
       // printf("Time after trigger %lf milliseconds\n",(double)(Camera.getTime()-to)/1000.0  );
    if(aabs(pts,to)>GAP)
    {
        printf("Error: Time difference: %d microseconds on interation: %d\n",aabs(pts,to),ct100*100+cntr);
        err++;
    }
    globalcounter1=myc1;
    if(cntr>100){
        ct100++;
        cout<<"Error count: "<<err-1<<" on "<<ct100*100<<" iterations."<<endl;
        cntr=0;
    }
    if(ct100>99)
        return 0;
    }
}
