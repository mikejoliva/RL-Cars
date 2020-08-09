#ifndef _NEURON_H
#define _NEURON_H

#include <vector>

class Neuron
{
private:
	float bias;
	float value;

	std::vector<Neuron*> next;
	std::vector<Neuron*> prev;

	std::vector<float> weights;

	inline float activation();

	Neuron() = delete;
public:
	Neuron(float);
	Neuron(const Neuron&);
	~Neuron();

	void setNext(std::vector<Neuron*>&, std::vector<float>&);
	void setPrev(std::vector<Neuron*>&);

	void addValue(float);
	void setValue(float);
	float getValue();

	std::vector<float>& getWeights();
	void setWeights(std::vector<float>&);

	float getBias();
	void setBias(float);
	void updateBias(float);

	void propagate();
};

#endif