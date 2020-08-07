#include "Layer.h"

Layer::Layer(unsigned int s) :
	prev(nullptr), next(nullptr)
{
	// Create s many neurons in this layer
	for (int idx = 0; idx < s; ++idx)
		neurons.push_back(Neuron(0.f));
}

Layer::~Layer()
{
	if (prev)
		delete prev;
	if (next)
		delete next;
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
		neurons[idx].setValue(0.f);
}

Neuron& Layer::getNeuron(size_t i)
{
	return neurons[i];
}

std::vector<Neuron>& Layer::getNeurons()
{
	return neurons;
}

size_t Layer::getSize()
{
	return neurons.size();
}