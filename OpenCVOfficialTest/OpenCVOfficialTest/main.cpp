
#include "OpenCVOfficialTest.h"

//using namespace cv;
//using namespace std;


int main(int argc, char* argv[])
//int _tmain(int argc, _TCHAR* argv[])
{
	OpenCVOfficialTest cvtest;
	//cvtest.opticalFlow();
	cvtest.createTrackbars();
	cvtest.trackDemBlobs();

	return 0;
}
