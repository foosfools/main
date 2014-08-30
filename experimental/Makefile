LIBS= -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab 

opticalFlowDect:
	g++ opticalFlowDect.cpp -O3 -I/usr/include/opencv $(LIBS) -o opticalFlowDect
	calibrate
clean:
	rm -rf *.o *~ $(EXECUTABLE) getPics foosfools

kalman:
	g++ kalmanFiltering.cpp -O1 -I/usr/include/opencv $(LIBS) -o kalman.o

calibrate: calibrate.cpp
	g++ calibrate.cpp -O3 -I/usr/include/opencv $(LIBS) -o calibrate

all: opticalFlowDect calibrate


