all:
	g++ main.cpp OpenCVOfficialTest.cpp Board.cpp motorController.cpp -O4 -I/usr/include/opencv -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab -lrt  -o NateIsABitch
	python -c "from Adafruit_BBIO.SPI import SPI; SPI(0,0)"	
clean:
	rm -rf *o test 
	
cleanPics:
	rm -rf *.png
