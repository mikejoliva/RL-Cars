#include <stdlib.h>
#include <random>
#include <array>
#include <iostream>
#include <algorithm>
#include <cstddef>

#include "network.hpp"

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
		layers[idx]->SetNext(layers[++idx]);

	SetupWeights();
}

Network::~Network()
{
	for (size_t idx = 0; idx < layers.size(); ++idx)
		if (layers[idx])
			delete layers[idx];
}

Direction Network::PredictMove(std::array<int, 5>& distances)
{
	// Input starting data into the input layer
	for (size_t idx = 0; idx < layers[0]->GetSize(); ++idx)
		layers[0]->GetNeuron(idx)->SetValue(distances.at(idx));

	// Remove existing values from the network
	for (size_t idx = 1; idx < layers.size(); ++idx)
		layers[idx]->Flush();

	// Propagate forwards in the network
	for (size_t outer = 0; outer < layers.size() - 1; ++outer)
		for (size_t inner = 0; inner < layers[outer]->GetSize(); ++inner)
			layers[outer]->GetNeuron(inner)->Propagate();
	
	// Find the move with the highest probability
	float highest = -1000.f;
	Direction move = Direction::STOP;
	for (size_t idx = 0; idx < layers[layers.size() - 1]->GetSize(); ++idx)
		if (layers[layers.size() - 1]->GetNeuron(idx)->GetValue() > highest)
		{
			highest = layers[layers.size() - 1]->GetNeuron(idx)->GetValue();
			move = static_cast<Direction>(idx);
		}
	
	//dump();

	return move;
}

void Network::Mutate(std::vector<Layer*>& l, float range)
{

	for (size_t idx = 0; idx < l.size(); ++idx)
		delete layers[idx];

	for (size_t idx = 0; idx < l.size(); ++idx)
		layers[idx] = new Layer(*l[idx]);

	for (size_t idx = 0; idx < layers.size() - 1;)
		layers[idx]->SetNext(layers[++idx]);

	// 'Nudge' our weights by a random value
	std::random_device dev;
	std::uniform_real_distribution<float> dist(-range, range);
	std::mt19937 mt(dev());

	Layer* current = layers[0];
	while (current->GetNext() != nullptr)
	{
		for (size_t idx = 0; idx < current->GetSize(); ++idx)
		{
			std::vector<float> weights = current->GetNeuron(idx)->GetWeights();
			for (size_t inner = 0; inner < current->GetNext()->GetSize(); ++inner)
				weights[inner] += dist(mt);
			current->GetNeuron(idx)->SetNext(current->GetNext()->GetNeurons(), weights);
			current->GetNeuron(idx)->UpdateBias(dist(mt) / 10.f);
		}

		current = current->GetNext();
	}

	//dump();
}

void Network::SetupWeights()
{
	// We need to assign a weight to every connection within each neuron
	// Seed with a real random value, if available
	std::random_device dev;
	std::uniform_real_distribution<float> dist(-5.f, 5.f);
	std::mt19937 mt(dev());

	// Link our nodes together
	Layer* current = layers[0];
	while (current->GetNext() != nullptr)
	{
		for (size_t idx = 0; idx < current->GetSize(); ++idx)
		{
			std::vector<float> weights;
			for (size_t inner = 0; inner < current->GetNext()->GetSize(); ++inner)
				weights.push_back(dist(mt));
			current->GetNeuron(idx)->SetNext(current->GetNext()->GetNeurons(), weights);
		}

		current = current->GetNext();
	}
}

std::vector<Layer*> Network::GetLayers()
{
	return layers;
}

void Network::Dump()
{
	std::cout << std::endl << std::endl <<  "Network dump: " << std::endl;

	int largest = 0;
	for (size_t idx = 0; idx < layers.size(); ++idx)
	{
		std::cout << "L " << idx << "(" << layers[idx]->GetSize() << ")" <<"\t";
		if (layers[idx]->GetSize() > largest)
			largest = layers[idx]->GetSize();
	}
	std::cout << std::endl;
	
	for (size_t count = 0; count <= largest; ++count)
	{
		for (size_t idx = 0; idx < layers.size(); ++idx)
			if (count < layers[idx]->GetSize())
				std::cout << layers[idx]->GetNeuron(count)->GetValue() << "\t";
		std::cout << std::endl;
	}

	std::cout << "Weight matrix: " << std::endl;
	for (size_t count = 0; count <= largest; ++count)
	{
		for (size_t idx = 0; idx < layers.size(); ++idx)
			if (count < layers[idx]->GetSize())
			{
				std::cout << "{";
				std::vector<float> weights = layers[idx]->GetNeuron(count)->GetWeights();
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
			if (count < layers[idx]->GetSize())
				std::cout << layers[idx]->GetNeuron(count) << "\t";
		std::cout << std::endl;
	}

	std::cout << std::endl << std::endl;
}
