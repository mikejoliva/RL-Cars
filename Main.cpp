#include <iostream>

#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <algorithm>

#include <SFML/Graphics.hpp>

#include "TrackInfo.h"
#include "EDirection.h"
#include "Car.h"

int main()
{
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "ML-Cars");
	// Limit the FPS as a lazy way to regulate the speed of the cars
	window.setFramerateLimit(144);

	// Setup the text writer
	sf::Font font;
	if (!font.loadFromFile("./font/open24.ttf"))
		return EXIT_FAILURE;
	sf::Text text;
	text.setFont(font); 
	text.setCharacterSize(32);
	text.setFillColor(sf::Color::Black);
	text.setPosition(10.f, 0.f);

	// Load the background track
	sf::Texture trackTexture;
	sf::Sprite track;
	if (!trackTexture.loadFromFile("./tracks/track1.png"))
		return EXIT_FAILURE;
	track.setTexture(trackTexture);

	// Keep an image of the track for pixel calculation
	sf::Image trackImage = trackTexture.copyToImage();

	TrackInfo trackInfo;
	if (trackInfo.loadTrackInfo("./tracks/track1.txt") != EXIT_SUCCESS)
		return EXIT_FAILURE;

	// Find our waypoints
	std::vector<std::vector<sf::Vertex>> waypoints = trackInfo.findWaypoints(trackImage);
	
	std::cout << "Generated " << waypoints.size() << " waypoints:" << std::endl;
	for (size_t idx = 0; idx < waypoints.size(); ++idx)
		std::cout << "\tWaypoint {" << std::to_string(idx) 
			<< "} A(" << waypoints[idx].at(0).position.x  << ", " << waypoints[idx].at(0).position.y 
			<< ") B(" << waypoints[idx].at(1).position.x << ", " << waypoints[idx].at(1).position.y
			<< ")" << std::endl;

	// Load our vehicle sprites
	const int CAR_COUNT = 25;
	std::vector<Car> cars;
	for (int idx = 0; idx < CAR_COUNT; ++idx)
		cars.push_back(Car(idx, trackInfo, trackImage, waypoints, 2, 5));

	std::cout << "Created " << cars.size() << " cars" << std::endl;

	// Find the FPS of the scene
	float fps;
	sf::Clock clock = sf::Clock::Clock();
	sf::Time previousTime = clock.getElapsedTime();
	sf::Time currentTime;

	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Clear screen
		window.clear();
			
		// Draw the background track
		window.draw(track);

		// Check if all cars are dead
		bool allDead = true;

		// Draw the cars + lines
		Car* bestCar = &cars[0];
		unsigned int bestScore = 0;
		for (std::vector<Car>::iterator car = cars.begin(); car != cars.end(); ++car)
		{
			// Let's display the stats of the car with the best 'score'
			if (car->getScore() > bestScore)
			{
				bestScore = car->getScore();
				bestCar = &(*car);
			}
				
			if (!car->isDead()) 
			{
				allDead = false;

				car->findMove();
				std::array<sf::Vertex[2], 5> lines = car->getLines();
				for (int idx = 0; idx < lines.size(); ++idx)
					window.draw(lines[idx], 2, sf::Lines);
			}
			window.draw(car->sprite);
		}

		if (allDead)
		{
			std::cout << "Best performing car was: " << bestCar->getID() << std::endl;
			std::vector<Layer*> bestLayers = bestCar->getNetwork()->getLayers();
			for (std::vector<Car>::iterator car = cars.begin(); car != cars.end(); ++car)
			{
				if (*car != bestCar)
					car->getNetwork()->mutate(bestLayers);

				car->reset();
			}
		}
			
				
		
		// Calculate FPS
		currentTime = clock.getElapsedTime();
		fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());
		previousTime = currentTime;

		text.setString(
			"FPS: " + std::to_string((int)fps) + '\n' +
			"Waypoints: " + std::to_string(bestCar->laps.waypointsPassed.size()) + '\n' +
			"Lap: " + std::to_string(bestCar->laps.lap)
		);

		window.draw(text);

		// REMOVEME: Debug movement keys
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			cars[0].move(EDirection::FORWARD);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			cars[0].move(EDirection::BACKWARD);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			cars[0].move(EDirection::ROTATE_LEFT);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			cars[0].move(EDirection::ROTATE_RIGHT);


		// Force reset ALL cars
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
			for (std::vector<Car>::iterator car = cars.begin(); car != cars.end(); ++car)
				car->reset();

		// Draw scene
		window.display();
	}

	return 0;
}