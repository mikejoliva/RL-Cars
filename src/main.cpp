#include "engine.hpp"

int main()
{
	Engine* engine = new Engine();
	engine->Run();

	delete engine;
}