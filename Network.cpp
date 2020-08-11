#include <stdlib.h>
#include <random>
#include <array>
#include <iostream>
#include <algorithm>

#include "Network.h"

Network::Network(unsigned int input, unsigned int output, unsigned int hidden, unsigned int neurons)
{
	// Create our neural network layers
	// Input layer
	layers.push_back(new Layer(input));
	// Hidden layers
	for (size_t idx = 0; idx < hidden; ++idx)
		layers.push_back(new Layer(neurons));

	// Output layer
	layers.push_back(new Layer(output));

	// Link each of the layers forwards
	for (size_t idx = 0; idx < layers.size() - 1;)
		layers[idx]->setNext(layers[++idx]);

	setupWeights();
}

Network::~Network()
{
	for (size_t idx = 0; idx < layers.size(); ++idx)
		if (layers[idx])
			delete layers[idx];
}

EDirection Network::predictMove(std::array<int, 5>& distances)
{
	// Input starting data into the input layer
	for (size_t idx = 0; idx < layers[0]->getSize(); ++idx)
		layers[0]->getNeuron(idx)->setValue(distances.at(idx));

	// Remove existing values from the network
	for (size_t idx = 1; idx < layers.size(); ++idx)
		layers[idx]->flush();

	// Propagate forwards in the network
	for (size_t outer = 0; outer < layers.size() - 1; ++outer)
		for (size_t inner = 0; inner < layers[outer]->getSize(); ++inner)
			layers[outer]->getNeuron(inner)->propagate();
	
	// Find the move with the highest probability
	float highest = -1000.f;
	EDirection move = EDirection::STOP;
	for (size_t idx = 0; idx < layers[layers.size() - 1]->getSize(); ++idx)
		if (layers[layers.size() - 1]->getNeuron(idx)->getValue() > highest)
		{
			highest = layers[layers.size() - 1]->getNeuron(idx)->getValue();
			move = static_cast<EDirection>(idx);
		}
	
	//dump();

	return move;
}

void Network::mutate(std::vector<Layer*>& l, float range)
{

	for (size_t idx = 0; idx < l.size(); ++idx)
		delete layers[idx];

	for (size_t idx = 0; idx < l.size(); ++idx)
		layers[idx] = new Layer(*l[idx]);

	for (size_t idx = 0; idx < layers.size() - 1;)
		layers[idx]->setNext(layers[++idx]);

	// 'Nudge' our weights by a random value
	std::random_device dev;
	std::uniform_real_distribution<float> dist(-range, range);
	std::mt19937 mt(dev());

	

	Layer* current = layers[0];
	while (current->getNext() != nullptr)
	{
		for (size_t idx = 0; idx < current->getSize(); ++idx)
		{
			std::vector<float> weights = current->getNeuron(idx)->getWeights();
			for (size_t inner = 0; inner < current->getNext()->getSize(); ++inner)
				weights[inner] += dist(mt);
			current->getNeuron(idx)->setNext(current->getNext()->getNeurons(), weights);
			current->getNeuron(idx)->updateBias(dist(mt) / 10.f);
		}

		current = current->getNext();
	}

	//dump();
}

void Network::setupWeights()
{
	// We need to assign a weight to every connection within each neuron
	// Seed with a real random value, if available
	std::random_device dev;
	std::uniform_real_distribution<float> dist(-5.f, 5.f);
	std::mt19937 mt(dev());

	// Link our nodes together
	Layer* current = layers[0];
	while (current->getNext() != nullptr)
	{
		for (size_t idx = 0; idx < current->getSize(); ++idx)
		{
			std::vector<float> weights;
			for (size_t inner = 0; inner < current->getNext()->getSize(); ++inner)
				weights.push_back(dist(mt));
			current->getNeuron(idx)->setNext(current->getNext()->getNeurons(), weights);
		}

		current = current->getNext();
	}
}

std::vector<Layer*> Network::getLayers()
{
	return layers;
}

void Network::dump()
{
	std::cout << std::endl << std::endl <<  "Network dump: " << std::endl;

	int largest = 0;
	for (size_t idx = 0; idx < layers.size(); ++idx)
	{
		std::cout << "L " << idx << "(" << layers[idx]->getSize() << ")" <<"\t";
		if (layers[idx]->getSize() > largest)
			largest = layers[idx]->getSize();
	}
	std::cout << std::endl;
	
	for (size_t count = 0; count <= largest; ++count)
	{
		for (size_t idx = 0; idx < layers.size(); ++idx)
			if (count < layers[idx]->getSize())
				std::cout << layers[idx]->getNeuron(count)->getValue() << "\t";
		std::cout << std::endl;
	}

	std::cout << "Weight matrix: " << std::endl;
	for (size_t count = 0; count <= largest; ++count)
	{
		for (size_t idx = 0; idx < layers.size(); ++idx)
			if (count < layers[idx]->getSize())
			{
				std::cout << "{";
				std::vector<float> weights = layers[idx]->getNeuron(count)->getWeights();
				for (size_t inner = 0; inner < weights.size(); ++inner)
				{
					std::cout << weights[inner];
					if (inner < weights.size() - 1)
						std::cout << ", ";
				}
				std::cout << "} \t";
			}
		std::cout << std::endl;
	}

	std::cout << "Reference dump: " << std::endl;
	for (size_t idx = 0; idx < layers.size(); ++idx)
		std::cout << "L " << idx << "(" << &layers[idx] << ")" << "\t";

	std::cout << std::endl;

	for (size_t count = 0; count <= largest; ++count)
	{
		for (size_t idx = 0; idx < layers.size(); ++idx)
			if (count < layers[idx]->getSize())
				std::cout << layers[idx]->getNeuron(count) << "\t";
		std::cout << std::endl;
	}

	std::cout << std::endl << std::endl;
}
