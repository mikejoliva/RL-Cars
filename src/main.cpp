#include <memory>

#include "engine.hpp"

int main()
{
	std::unique_ptr<Engine> engine = std::make_unique<Engine>();
	engine->Run();
}