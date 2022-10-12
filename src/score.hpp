#ifndef _SCORE_H
#define _SCORE_H

#include <vector>

#include "car.hpp"

class Score
{
private:
    std::vector<Car>* cars;
	unsigned int generation;
    sf::Clock clock;
	sf::Time previousTime;
	sf::Time currentTime;
    Score() = delete;
public:
    Score(std::vector<Car>* cars);
    void Reset();
    bool AllCarsDead();
    bool AllCarsFailedToScore();
    void NewGeneration();
    unsigned int GetGeneration();
    unsigned int GetAlive();
    unsigned int GetFPS();
    Car& GetBestCar();
};

#endif