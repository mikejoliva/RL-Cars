#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <string>
#include <vector>

class Configuration
{
private:
    struct Config 
    {
    public:
        std::string font;
        std::string track;
        int threads;
        int carsPerThread;
        int networkHiddenCount;
        int networkNeuronCount;
        float carMoveSpeed;
        float carRotateSpeed;
        float carMaxSpeedMultiplier;
        int windowHeight;
        int windowWidth;
        bool drawInputLines;
        bool dumpBestNetwork;
    } config;

    const std::string FONT = "FONT";
    const std::string TRACK = "TRACK";
    const std::string THREADS = "THREADS";
    const std::string CARS_PER_THREAD = "CARS_PER_THREAD";
    const std::string NETWORK_INPUT_COUNT = "NETWORK_INPUT_COUNT";
    const std::string NETWORK_OUTPUT_COUNT = "NETWORK_OUTPUT_COUNT";
    const std::string NETWORK_HIDDEN_COUNT = "NETWORK_HIDDEN_COUNT";
    const std::string NETWORK_NEURON_COUNT = "NETWORK_NEURON_COUNT";
    const std::string CAR_MOVE_SPEED = "CAR_MOVE_SPEED";
    const std::string CAR_ROTATE_SPEED = "CAR_ROTATE_SPEED";
    const std::string CAR_MAX_SPEED_MULTIPLIER = "CAR_MAX_SPEED_MULTIPLIER";
    const std::string DRAW_INPUT_LINES = "DRAW_INPUT_LINES";
    const std::string DUMP_BEST_NETWORK = "DUMP_BEST_NETWORK";
    const std::string WINDOW_WIDTH = "WINDOW_WIDTH";
    const std::string WINDOW_HEIGHT = "WINDOW_HEIGHT";

    const std::string CONFIG_PATH = "./setup.config";

    std::vector<std::string> Split(std::string line, char delim);
public:
    Configuration();
    void Parse();

    std::string GetFont();
    std::string GetTrack();
    int GetThreadCount();
    int GetCarsPerThread();
    int GetNetworkInputCount();
    int GetNetworkOutputCount();
    int GetNetworkHiddenCount();
    int GetNetworkNeuronCount();
    float GetCarMoveSpeed();
    float GetCarRotateSpeed();
    float GetCarMaxSpeedMultiplier();
    int GetWindowWidth();
    int GetWindowHeight();
    bool ShouldDrawLines();
    bool ShouldDumpBestNetwork();
};

#endif