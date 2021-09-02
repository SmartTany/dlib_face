main: main.o yoloface.o
	g++ -o main main.o yoloface.o
main.o: main.cpp
	g++ -c main.cpp
yoloface.o: dlibface.cpp
	g++ -c dlibface.cpp

#g++ -o main main.cpp yoloface.cpp -I /usr/local/include -I /usr/local/include/opencv -I /usr/local/include/opencv2 -L /usr/local/lib /usr/local/lib/*.so

clean:
	rm *.o
	rm main.o
