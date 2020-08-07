#include "Neuron.h"

#include <math.h>

inline float Neuron::activation()
{
	// Approximate sigmoid 
	return  value / (1 + abs(value));
}

Neuron::Neuron(float b) :
	bias(b), value(0.f)
{ /* Empty */ }

Neuron::Neuron(Neuron& n) :
	bias(n.bias),
	value(n.value),
	next(n.next),
	prev(n.prev),
	weights(n.weights)
{
}

Neuron::~Neuron()
{
	/*
	for (int idx = 0; idx < prev.size(); ++idx)
		if (prev[idx])
			delete prev[idx];

	for (int idx = 0; idx < next.size(); ++idx)
		if (next[idx])
			delete next[idx];
			*/
}

void Neuron::setNext(std::vector<Neuron>& v, std::vector<float>& w)
{
	next = v;
	weights = w;
}

void Neuron::setPrev(std::vector<Neuron>& v)
{
	prev = v;
}

void Neuron::addValue(float val)
{
	value += val;
}

void Neuron::setValue(float val)
{
	value = val;
}

float Neuron::getValue()
{
	return activation() + bias;
}

std::vector<float>& Neuron::getWeights()
{
	return weights;
}

void Neuron::setWeights(std::vector<float>& w)
{
	weights = w;
}

void Neuron::updateBias(float b)
{
	bias = b;
}

void Neuron::propagate()
{
	for (int idx = 0; idx < next.size(); ++idx)
		next[idx].addValue(getValue() * weights[idx]);
}

