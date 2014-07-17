LIBS= -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab 


all:
	g++ opticalFlowDect.cpp -O3 -I/usr/include/opencv $(LIBS) -o opticalFlowDect

clean:
	rm -rf *.o *~ $(EXECUTABLE) getPics foosfools

calibrate:
	g++ calibrate.cpp -O3 -I/usr/include/opencv $(LIBS) -o calibrate


