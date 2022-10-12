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

	void SetNext(std::vector<Neuron*>&, std::vector<float>&);
	void SetPrev(std::vector<Neuron*>&);

	void AddValue(float);
	void SetValue(float);
	float GetValue();

	std::vector<float>& GetWeights();
	void SetWeights(std::vector<float>&);

	float GetBias();
	void SetBias(float);
	void UpdateBias(float);

	void Propagate();
};

#endif