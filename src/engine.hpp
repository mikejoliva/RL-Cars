#ifndef _ENGINE_H
#define _ENGINE_H

#include <vector>

#include <SFML/Graphics.hpp>

#include "configuration.hpp"
#include "trackinfo.hpp"
#include "direction.hpp"
#include "car.hpp"
#include "carbatch.hpp"
#include "score.hpp"

class Engine
{
private:
    const std::string WINDOW_TITLE = "RL-Cars";

    Configuration* config;
    Score* score;

    sf::RenderWindow* window;

    sf::Font font;
    sf::Text text;
    sf::Texture trackTexture;
	sf::Sprite track;
    sf::Image trackImage;

    TrackInfo trackInfo;
    std::vector<std::vector<sf::Vertex>> waypoints;

    size_t CAR_THREAD_COUNT;
    size_t CARS_PER_THREAD;
    size_t NETWORK_INPUT_COUNT;
    size_t NETWORK_OUTPUT_COUNT;
    size_t NETWORK_HIDDEN_COUNT;
    size_t NETWORK_NEURON_COUNT;

    std::vector<Car> cars;
    std::vector<std::thread> threads;
    std::vector<CarBatch> batches;

    sf::Event event;

    void Setup();
    void Event();
    void Update();
    void Overlay();
    void Render();
    void Kill();
public:
    Engine();
    ~Engine();
    void Run();
};

#endif