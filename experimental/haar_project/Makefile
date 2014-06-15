LIBS= -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_ts -lopencv_video -lopencv_videostab 


tracking:
	g++ ourDetect.cpp -O3 -I/usr/include/opencv $(LIBS) -o ourDetect

getPics:
	g++ ff.cpp -O3 -I/usr/include/opencv $(LIBS) -o getPics

clean:
	rm -rf *.o -f positive_images/*.jpg negative_images/*.jpg bg.txt info.dat *~ $(EXECUTABLE)

cleanNegs:
	rm -f negative_images/*.jpg
	rm bg.txt	

cleanPos:
	rm -f info.dat positive_images/*.jpg

createSamples:
	opencv_createsamples -vec positiveSamples.vec -info info.dat -bg bg.txt -num 37 -bgcolor 0 -bgthresh 0 -maxxangle 0.71 -maxyangle 0.71 -maxidev 40 -w 30 -h 120	-show
