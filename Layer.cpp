#include "Layer.h"

#include <random>

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

void Layer::setNext(Layer* l)
{
	next = l;
}

void Layer::setPrev(Layer* l)
{
	prev = l;
}

Layer* Layer::getNext()
{
	return next;
}

Layer* Layer::getPrev()
{
	return prev;
}

void Layer::flush()
{
	for (int idx = 0; idx < neurons.size(); ++idx)
		neurons[idx]->setValue(0.f);
}

Neuron* Layer::getNeuron(size_t i)
{
	return neurons[i];
}

std::vector<Neuron*>& Layer::getNeurons()
{
	return neurons;
}

size_t Layer::getSize()
{
	return neurons.size();
}