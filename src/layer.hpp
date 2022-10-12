#ifndef _LAYER_H
#define _LAYER_H

#include <cstddef>
#include <vector>

#include "neuron.hpp"

class Layer
{
private:
	std::vector<Neuron*> neurons;

	Layer* prev;
	Layer* next;

	Layer() = delete;
public:
	Layer(unsigned int);
	Layer(const Layer&);

	~Layer();
	
	void SetNext(Layer*);
	void SetPrev(Layer*);

	Layer* GetNext();
	Layer* GetPrev();

	void Flush();

	Neuron* GetNeuron(size_t);
	std::vector<Neuron*>& GetNeurons();
	
	size_t GetSize();
};

#endif
