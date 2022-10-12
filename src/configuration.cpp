#include <assert.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "configuration.hpp"

Configuration::Configuration() { /* Empty */ }

std::vector<std::string> Configuration::Split(std::string line, char delim)
{
    std::vector<std::string> tokens;
    std::string token = "";
    for(char& c : line) {
        if (c == delim) {
            tokens.push_back(token);
            break;
        }
        token += c;
    }
    tokens.push_back(line.substr(token.length() + 1));
    return tokens;
}

void Configuration::Parse()
{
    std::ifstream file(CONFIG_PATH);
    for (std::string line; std::getline(file, line); ) 
    {
        std::vector<std::string> parts = Split(line, '=');
        assert(parts.size() == 2);
        std::string key = parts[0];
        std::string value = parts[1];

        if (key == FONT)
        {
            config.font = value;
            continue;
        }
        if (key == TRACK)
        {
            config.track = value;
            continue;
        }
        if (key == THREADS)
        {
            config.threads = std::stoi(value);
            continue;
        }
        if (key == CARS_PER_THREAD)
        {
            config.carsPerThread = std::stoi(value);
            continue;
        }
        if (key == NETWORK_HIDDEN_COUNT)
        {
            config.networkHiddenCount = std::stoi(value);;
            continue;
        }
        if (key == NETWORK_NEURON_COUNT)
        {
            config.networkNeuronCount = std::stoi(value);;
            continue;
        }
        if (key == CAR_MOVE_SPEED)
        {
            config.carMoveSpeed = std::stof(value);
            continue;
        }
        if (key == CAR_ROTATE_SPEED)
        {
            config.carRotateSpeed = std::stof(value);;
            continue;
        }
        if (key == CAR_MAX_SPEED_MULTIPLIER)
        {
            config.carMaxSpeedMultiplier = std::stof(value);;
            continue;
        }
        if (key == WINDOW_WIDTH)
        {
            config.windowWidth = std::stoi(value);;
            continue;
        }
        if (key == WINDOW_HEIGHT)
        {
            config.windowHeight = std::stoi(value);;
            continue;
        }
        if (key == DRAW_INPUT_LINES)
        {
            config.drawInputLines = std::stoi(value);;
            continue;
        }
        if (key == DUMP_BEST_NETWORK)
        {
            config.dumpBestNetwork = std::stoi(value);;
            continue;
        }
    }
}

std::string Configuration::GetFont() 
{
    return config.font;
}

std::string Configuration::GetTrack() 
{
    return config.track;
}

int Configuration::GetThreadCount() 
{
    return config.threads;
}

int Configuration::GetCarsPerThread() 
{
    return config.carsPerThread;
}

int Configuration::GetNetworkHiddenCount() 
{
    return config.networkHiddenCount;
}

int Configuration::GetNetworkNeuronCount() 
{
    return config.networkNeuronCount;
}

float Configuration::GetCarMoveSpeed()
{
    return config.carMoveSpeed;
}

float Configuration::GetCarRotateSpeed()
{
    return config.carRotateSpeed;
}

float Configuration::GetCarMaxSpeedMultiplier()
{
    return config.carMaxSpeedMultiplier;
}

int Configuration::GetWindowWidth()
{
    return config.windowWidth;
}

int Configuration::GetWindowHeight()
{
    return config.windowHeight;
}

bool Configuration::ShouldDrawLines() 
{
    return config.drawInputLines;
}

bool Configuration::ShouldDumpBestNetwork() 
{
    return config.dumpBestNetwork;
}