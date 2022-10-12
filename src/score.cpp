#include "score.hpp"

Score::Score(std::vector<Car>* cars)
{
    this->cars = cars;
	previousTime = clock.getElapsedTime();

    Reset();
}

void Score::Reset()
{
	generation = 0;
}

bool Score::AllCarsDead()
{
    return GetAlive() == 0;
}

bool Score::AllCarsFailedToScore()
{
    // Score will be <= 1 if all cars failed to score
    // as the start line counts as a point.
    return GetBestCar().GetScore() <= 1;
}

void Score::NewGeneration()
{
    ++generation;
}

unsigned int Score::GetGeneration()
{
    return generation;
}

unsigned int Score::GetAlive()
{
    unsigned int alive = 0;
    for (std::vector<Car>::iterator car = cars->begin(); car != cars->end(); ++car)
        if (!car->IsDead()) 
			++alive;
            
    return alive;
}

unsigned int Score::GetFPS()
{
    // Calculate FPS
    currentTime = clock.getElapsedTime();
    float fps = 1.0f / (currentTime.asSeconds() - previousTime.asSeconds());
    previousTime = currentTime;
    return static_cast<unsigned int>(fps);
}

Car& Score::GetBestCar()
{
    Car* bestCar = &cars->at(0);
    for (std::vector<Car>::iterator car = cars->begin(); car != cars->end(); ++car)
    {
        if (car->GetScore() < bestCar->GetScore())
            continue;

        if (car->GetScore() > bestCar->GetScore())
        {
            bestCar = &(*car);
            continue;
        }

        // The score is equal -> Get the faster car
        if (car->GetScore() == bestCar->GetScore() && car->GetTimeAlive() < bestCar->GetTimeAlive())
            bestCar = &(*car);
    }

    return *bestCar;
}