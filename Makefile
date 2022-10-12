compile:
	g++ -c ./src/*.cpp 
	mkdir -p ./build/objects
	mkdir -p ./build/tracks
	mkdir -p ./build/font
	mkdir -p ./build/images
	mv *.o ./build/objects
	cp ./tracks/* ./build/tracks
	cp ./font/* ./build/font
	cp ./images/* ./build/images
	cp setup.config ./build
	g++ ./build/objects/*.o -o ./build/rl-cars -lsfml-graphics -lsfml-window -lsfml-system

run:
	./build/rl-cars

go: compile run