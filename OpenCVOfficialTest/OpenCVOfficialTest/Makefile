LIBS := -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab #-lrt

INCLUDES := /usr/include/opencv

DANCE_FILES:=DancingMotors.cpp
DANCE_FILES+=main2.cpp
DANCE_FILES+=../../MotorBoard/virtualPort/virtualPort.cpp

C_SRC_FILES := OpenCVOfficialTest.cpp
C_SRC_FILES += main.cpp
C_SRC_FILES += Board.cpp
C_SRC_FILES += ../../MotorBoard/virtualPort/virtualPort.cpp
#C_SRC_FILES += motorController.cpp

C_FLAGS:=-stdlib=libstdc++ -std=c++11 -O3

all:
	g++ $(C_FLAGS) $(C_SRC_FILES) -I $(INCLUDES) $(LIBS) -o foosfools
	size foosfools
clean:
	rm -rf *.o *~ $(EXECUTABLE)
cleanPics:
	rm -rf *.png

dance:
		g++ $(C_FLAGS) $(DANCE_FILES) -I $(INCLUDES) $(LIBS) -o dance.o
	size dance.o

