LIBS := -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab #-lrt

INCLUDES := /usr/include/opencv

C_SRC_FILES := OpenCVOfficialTest.cpp
C_SRC_FILES += main.cpp
C_SRC_FILES += Board.cpp
#C_SRC_FILES += motorController.cpp


all:
	g++ $(C_SRC_FILES) -O4 -I $(INCLUDES) $(LIBS) -o foosfools

clean:
	rm -rf *.o *~ $(EXECUTABLE)
cleanPics:
	rm -rf *.png
