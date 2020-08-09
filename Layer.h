#ifndef _LAYER_H
#define _LAYER_H

#include <vector>

#include "Neuron.h"

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
	
	void setNext(Layer*);
	void setPrev(Layer*);

	Layer* getNext();
	Layer* getPrev();

	void flush();

	Neuron* getNeuron(size_t);
	std::vector<Neuron*>& getNeurons();
	
	size_t getSize();
};

#endif
