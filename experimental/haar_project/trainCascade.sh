opencv_traincascade -data data -vec positiveSamples.vec -bg bg.txt -numPos 44 -numNeg 622 -numStages 20 -precalcValBufSize 256 -precalcIdxBufSize 256 -w 30 -h 120 -mode ALL -maxFalseAlarmRate 0.5 -minHitRate 0.999
