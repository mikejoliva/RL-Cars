#include <math.h>

#include "neuron.hpp"

inline float Neuron::activation()
{
	// Approximate sigmoid 
	return  value / (1 + abs(value));
}

Neuron::Neuron(float b) :
	bias(b), value(0.f)
{ /* Empty */ }

Neuron::Neuron(const Neuron& n) :
	bias(n.bias),
	value(n.value),
	next(n.next),
	prev(n.prev),
	weights(n.weights)
{
}

Neuron::~Neuron()
{
	prev.clear();
	next.clear();
}

void Neuron::SetNext(std::vector<Neuron*>& v, std::vector<float>& w)
{
	next = v;
	weights = w;
}

void Neuron::SetPrev(std::vector<Neuron*>& v)
{
	prev = v;
}

void Neuron::AddValue(float val)
{
	value += val;
}

void Neuron::SetValue(float val)
{
	value = val;
}

float Neuron::GetValue()
{
	return activation() + bias;
}

std::vector<float>& Neuron::GetWeights()
{
	return weights;
}

void Neuron::SetWeights(std::vector<float>& w)
{
	weights = w;
}

float Neuron::GetBias()
{
	return bias;
}

void Neuron::SetBias(float b)
{
	bias = b;
}

void Neuron::UpdateBias(float b)
{
	bias += b;
}

void Neuron::Propagate()
{
	for (int idx = 0; idx < next.size(); ++idx)
		next[idx]->AddValue(GetValue() * weights[idx]);
}

