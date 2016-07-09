/**
 * Originally Raspicam-0.1.3
 * Modified into OurRaspicam-1.0.0
 * Fixed many bugs and added functionality
 * By:  ThreePixelsTeam(Subrato Chakraborty, Om Sahoo and Piyush Soni)
 * 		IIT Varanasi
 * 		 
 */ 		

/**********************************************************
 Software developed by AVA ( Ava Group of the University of Cordoba, ava  at uco dot es)
 Main author Rafael Munoz Salinas (rmsalinas at uco dot es)
 This software is released under BSD license as expressed below
-------------------------------------------------------------------
Copyright (c) 2013, AVA ( Ava Group University of Cordoba, ava  at uco dot es)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:

   This product includes software developed by the Ava group of the University of Cordoba.

4. Neither the name of the University nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AVA ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AVA BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************/

#include "raspicam_cv.h"
#include "private/private_impl.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "scaler.h"
namespace raspicam {
RaspiCam_Cv::RaspiCam_Cv() {
    _impl = new _private::Private_Impl();
    set(CV_CAP_PROP_FORMAT, CV_8UC3);

}
RaspiCam_Cv::~RaspiCam_Cv() {
    delete _impl;
}

/** Open  capturing device for video capturing
 */
bool RaspiCam_Cv::open ( void ) {
    return _impl->open();
}
/**
 * Returns true if video capturing has been initialized already.
 */
bool RaspiCam_Cv::isOpened() const {return _impl->isOpened();}
/**
*Closes video file or capturing device.
*/
void RaspiCam_Cv::release() {
    _impl->release();
}

/**
 * Grabs the next frame from video file or capturing device.
 */
bool RaspiCam_Cv::grab() {
    return _impl->grab();
}

/**
 * gives the correct time in microseconds
 */
uint64_t RaspiCam_Cv::getTime() {
    uint64_t time = _impl->gettime();
    return time;
}
/**
*Decodes and returns the grabbed video frame.
 */
void RaspiCam_Cv::retrieve ( cv::Mat& image, uint64_t& pts ) {
    //here we go!
    image.create ( _impl->getHeight(), _impl->getWidth(), imgFormat );
    _impl->retrieve ( image.ptr<uchar> ( 0 ), pts);
}

/**Returns the specified VideoCapture property
 */

double RaspiCam_Cv::get ( int propId ) {

    switch ( propId ) {

    case CV_CAP_PROP_FRAME_WIDTH :
        return _impl->getWidth();
    case CV_CAP_PROP_FRAME_HEIGHT :
        return _impl->getHeight();
    case CV_CAP_PROP_FPS:
        return 30;
    case CV_CAP_PROP_FORMAT :
        return imgFormat;
    case CV_CAP_PROP_MODE :
        return 0;
    case CV_CAP_PROP_BRIGHTNESS :
        return _impl->getBrightness();
    case CV_CAP_PROP_CONTRAST :
        return Scaler::scale ( -100, 100, 0, 100,  _impl->getContrast() );
    case CV_CAP_PROP_SATURATION :
        return  Scaler::scale ( -100, 100, 0, 100, _impl->getSaturation() );;
    case CV_CAP_PROP_HUE :
        return Scaler::scale ( -100,100,0,100,  _impl->getSharpness() ); 
    case CV_CAP_PROP_GAIN :
        return  Scaler::scale ( 0, 800, 0, 100, _impl->getISO() );
    case CV_CAP_PROP_EXPOSURE :
        if ( _impl->getShutterSpeed() == 0 )
            return -1;//auto
        else return Scaler::scale (0, 330000, 0, 330, _impl->getShutterSpeed() )  ;
        break;
    case CV_CAP_PROP_CONVERT_RGB :
        return ( imgFormat == CV_8UC3 );
    case CV_CAP_PROP_WHITE_BALANCE_RED_V:
        return _impl->getAWBG_red() * 100;
        break;

    case CV_CAP_PROP_WHITE_BALANCE_BLUE_U:
        return _impl->getAWBG_blue() * 100;
        break;
    default :
        return -1;
    };
}

/**Sets a property in the VideoCapture.
 */

bool RaspiCam_Cv::set ( int propId, double value ) {

    switch ( propId ) {

    case CV_CAP_PROP_FRAME_WIDTH :
        _impl->setWidth ( value );
        break;
    case CV_CAP_PROP_FRAME_HEIGHT :
        _impl->setHeight ( value );
        break;
    case CV_CAP_PROP_FORMAT : {
        bool res = true;
        if ( value == CV_8UC1  ) {
            _impl->setFormat(RASPICAM_FORMAT_GRAY);
            imgFormat = value;
        }
        else if (value == CV_8UC3) {
            _impl->setFormat(RASPICAM_FORMAT_BGR);
            imgFormat = value;
        }
        else res = false; //error int format
        return res;
    } break;
    case CV_CAP_PROP_MODE ://nothing to  do yet
        return false;
        break;
    case CV_CAP_PROP_BRIGHTNESS :
        _impl->setBrightness ( value );
        break;
    case CV_CAP_PROP_CONTRAST :
        _impl->setContrast ( Scaler::scale ( 0, 100, -100, 100, value ) );
        break;
    case CV_CAP_PROP_SATURATION :
        _impl->setSaturation ( Scaler::scale ( 0, 100, -100, 100, value ) );
        break;
    case CV_CAP_PROP_HUE :
        _impl->setSharpness ( Scaler::scale ( 0,100,-100,100, value ) );
        break;
    case CV_CAP_PROP_GAIN :
        _impl->setISO ( Scaler::scale ( 0, 100, 0, 800, value ) );
        break;
    case CV_CAP_PROP_EXPOSURE :
        if ( value > 0 && value <= 100 ) {
            _impl->setShutterSpeed ( Scaler::scale ( 0, 330, 0, 330000, value ) );
        } else {
            _impl->setExposure ( RASPICAM_EXPOSURE_AUTO );
            _impl->setShutterSpeed ( 0 );
        }
        break;
    case CV_CAP_PROP_CONVERT_RGB :
        imgFormat = CV_8UC3;
        break;
    case CV_CAP_PROP_WHITE_BALANCE_RED_V:
        if (value == 0) _impl->setAWB(raspicam::RASPICAM_AWB_AUTO);
        else  {
              double valblue=_impl->getAWBG_blue();
              _impl->setAWB(raspicam::RASPICAM_AWB_OFF);
              _impl->setAWB_RB(value/100.0,valblue);
        };
        break;

    case CV_CAP_PROP_WHITE_BALANCE_BLUE_U:
        if (value == 0) _impl->setAWB(raspicam::RASPICAM_AWB_AUTO);
        else  {
             double valred = _impl->getAWBG_red();
            _impl->setAWB(raspicam::RASPICAM_AWB_OFF);
            _impl->setAWB_RB(valred, value/100.0 );
        };

        break;

//     case CV_CAP_PROP_WHITE_BALANCE :return _cam_impl->getAWB();
    default :
        return false;
    };
    return true;

}
std::string RaspiCam_Cv::getId() const {
    return _impl->getId();
}

}


