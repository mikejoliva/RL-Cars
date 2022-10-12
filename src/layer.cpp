#include <cstddef>
#include <random>

#include "layer.hpp"

Layer::Layer(unsigned int s) :
	prev(nullptr), next(nullptr)
{
	std::random_device dev;
	std::uniform_real_distribution<float> dist(-.5f, .5f);
	std::mt19937 mt(dev());

	// Create s many neurons in this layer
	for (int idx = 0; idx < s; ++idx)
		neurons.push_back(new Neuron(dist(mt)));
}

Layer::Layer(const Layer& l) : 
	prev(nullptr), next(nullptr)
{
	neurons.resize(l.neurons.size());
	for (size_t idx = 0; idx < l.neurons.size(); ++idx)
		neurons[idx] = new Neuron(*l.neurons[idx]);

	if (next)
		next = new Layer(*l.next);
	if (prev)
		prev = new Layer(*l.prev);
}

Layer::~Layer()
{
	for (size_t idx = 0; idx < neurons.size(); ++idx)
		if (neurons[idx])
			delete neurons[idx];

	neurons.clear();
}

void Layer::SetNext(Layer* l)
{
	next = l;
}

void Layer::SetPrev(Layer* l)
{
	prev = l;
}

Layer* Layer::GetNext()
{
	return next;
}

Layer* Layer::GetPrev()
{
	return prev;
}

void Layer::Flush()
{
	for (int idx = 0; idx < neurons.size(); ++idx)
		neurons[idx]->SetValue(0.f);
}

Neuron* Layer::GetNeuron(size_t i)
{
	return neurons[i];
}

std::vector<Neuron*>& Layer::GetNeurons()
{
	return neurons;
}

size_t Layer::GetSize()
{
	return neurons.size();
}