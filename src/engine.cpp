#include <assert.h>
#include <iostream>
#include <cstddef>
#include <string>
#include <array>
#include <thread>

#include "engine.hpp"

Engine::Engine()
{
    Setup();
}

void Engine::Setup()
{
	cars.clear();
	batches.clear();
	threads.clear();
	waypoints.clear();

    std::cout << "Loading configuration" << std::endl;
    config = new Configuration();
	config->Parse();

    std::cout << "Loading font" << std::endl;
    assert(font.loadFromFile(config->GetFont()));
	text.setFont(font); 
	text.setCharacterSize(32);
	text.setFillColor(sf::Color::Black);
	text.setPosition(10.f, 0.f);

    std::cout << "Loading track image: ./tracks/" << config->GetTrack() << ".png" << std::endl;
    assert(trackTexture.loadFromFile("./tracks/" + config->GetTrack() + ".png"));
	track.setTexture(trackTexture);

    // Keep an image of the track for pixel calculation
	trackImage = trackTexture.copyToImage();

    std::cout << "Loading track info: ./tracks/" << config->GetTrack() << ".ini" << std::endl;
    assert(trackInfo.LoadTrackInfo("./tracks/" + config->GetTrack() + ".ini") == EXIT_SUCCESS);

    std::cout << "Finding track waypoints" << std::endl;
    waypoints = trackInfo.FindWaypoints(trackImage);
    assert(waypoints.size() != 0);
    std::cout << "Found " << waypoints.size() << " waypoints" << std::endl;
	for (size_t idx = 0; idx < waypoints.size(); ++idx)
		std::cout << "\tWaypoint {" << std::to_string(idx) 
			<< "} A(" << waypoints[idx].at(0).position.x  << ", " << waypoints[idx].at(0).position.y 
			<< ") B(" << waypoints[idx].at(1).position.x << ", " << waypoints[idx].at(1).position.y
			<< ")" << std::endl;

    std::cout << "Creating cars and threads" << std::endl;
    CAR_THREAD_COUNT = config->GetThreadCount();
	CARS_PER_THREAD = config->GetCarsPerThread();

	NETWORK_INPUT_COUNT = 4;
	NETWORK_OUTPUT_COUNT = 5;
	
	NETWORK_HIDDEN_COUNT = config->GetNetworkHiddenCount();
	NETWORK_NEURON_COUNT = config->GetNetworkNeuronCount();

    for (size_t idx = 0; idx < CAR_THREAD_COUNT * CARS_PER_THREAD; ++idx)
	{
		Network* network = new Network(
			NETWORK_INPUT_COUNT,
			NETWORK_OUTPUT_COUNT,
			NETWORK_HIDDEN_COUNT,
			NETWORK_NEURON_COUNT
		);
		cars.push_back(Car(config, idx, trackInfo, trackImage, waypoints, network));
	}

    std::cout << "Allocating cars to batches" << std::endl;
    for (size_t idx = 0; idx < CAR_THREAD_COUNT; ++idx)
	{
		std::vector<std::function<void()>> funcs(CARS_PER_THREAD);
		for (size_t inner = 0; inner < CARS_PER_THREAD; ++inner)
		{
			funcs[inner] = std::function<void()>([this, idx, inner] {
				this->cars[(this->CARS_PER_THREAD * idx) + inner].Run();
			});
		}

		batches.push_back(CarBatch(funcs));
	}

    std::cout << "Allocating batches to threads" << std::endl;
    for (size_t idx = 0; idx < CAR_THREAD_COUNT; ++idx)
    {
		threads.push_back(std::thread([this, idx] {
			this->batches[idx].Run();
		}));
    }
	std::cout << "Created " << cars.size() << " cars" << std::endl;

	std::cout << "Initialising score" << std::endl;
	score = new Score(&cars);

    std::cout << "Creating window" << std::endl;
    window = new sf::RenderWindow(sf::VideoMode(config->GetWindowWidth(), config->GetWindowHeight()), WINDOW_TITLE);

    std::cout << "Setup complete!" << std::endl;
}

Engine::~Engine()
{
    Kill();
}

void Engine::Kill()
{
    window->close();

    // Kill the threads
    for (size_t idx = 0; idx < batches.size(); ++idx)
        batches[idx].Stop();
    for (auto& t : threads)
        t.join();

    delete config;
	delete score;
	delete window;
}

void Engine::Run()
{
    while (window->isOpen())
	{
        Event();
        Update();
        Overlay();
        Render();
    }
}

void Engine::Render()
{
    window->display();
}

void Engine::Overlay()
{
    text.setString(
        "FPS: " + std::to_string(score->GetFPS()) + '\n' +
        "Generation: " + std::to_string(score->GetGeneration()) + '\n' +
        "Alive: " + std::to_string(score->GetAlive()) + "/" + std::to_string(CAR_THREAD_COUNT * CARS_PER_THREAD)
    );
    window->draw(text);
}

void Engine::Update()
{
	// Clear screen
	window->clear();
		
	// Draw the background track
	window->draw(track);

	// Draw the cars + lines
	for (std::vector<Car>::iterator car = cars.begin(); car != cars.end(); ++car)
	{
		if (config->ShouldDrawLines() && !car->IsDead()) 
		{
			std::array<sf::Vertex[2], 5> lines = car->GetLines();
			for (int idx = 0; idx < lines.size(); ++idx)
				window->draw(lines[idx], 2, sf::Lines);
		}
		window->draw(car->GetSprite());
	}

	if (!score->AllCarsDead())
		return;

	if (score->AllCarsFailedToScore())
	{
		std::cout << "All cars failed to score! Creating new weights..." << std::endl;
		for (std::vector<Car>::iterator car = cars.begin(); car != cars.end(); ++car)
		{
			car->GetNetwork()->SetupWeights();
			car->Reset();
		}
		score->Reset();
		return;
	}

	Car& bestCar = score->GetBestCar();
	std::cout << "Best performing car was: " << bestCar.GetID()
		<< " | Score: " << bestCar.GetScore()
		<< " | Time alive: " << bestCar.GetTimeAlive() << "s" << std::endl;

	if (config->ShouldDumpBestNetwork())
		bestCar.GetNetwork()->Dump();

	std::vector<Layer*> bestLayers = bestCar.GetNetwork()->GetLayers();
	for (std::vector<Car>::iterator car = cars.begin(); car != cars.end(); ++car)
	{
		if (*car != bestCar)
			car->GetNetwork()->Mutate(bestLayers, .5f);
		car->Reset();
	}

	score->NewGeneration();
}

void Engine::Event()
{
    while (window->pollEvent(event))
    {
        // Close window, exit
        if (event.type == sf::Event::Closed)
        {
           Kill();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::R)
            {
                std::cout << "Reloading..." << std::endl;
                Kill();
                Setup();
            }
        }
    }
}