#ifndef _CAR_H
#define _CAR_H

#include <array>
#include <vector>
#include <functional>
#include <cstddef>

#include "network.hpp"
#include "trackinfo.hpp"
#include "direction.hpp"
#include "configuration.hpp"

class Car
{
private:
	const float pi = 3.14159;
	Configuration* config;
	std::array<sf::Vertex[2], 5> lines;
	sf::FloatRect previousBounds;
	sf::Vector2f previousPos;
	size_t moveCount = 0;
	unsigned int lastScore = 0;
	std::vector<std::vector<sf::Vertex>> waypoints;
	sf::Vector2f points[8];

	// Updated within the Move function
	sf::FloatRect rect;
	sf::Transform trans;

	sf::Texture* texture;
	sf::Sprite sprite;

	unsigned int id;
	bool dead = false;
	float moveSpeed;
	float rotateSpeedRatio;
	float maxSpeed;
	float speed = 0.f;

	// Store a copy of the track info and image
	TrackInfo& trackInfo;
	sf::Image& track;

	Network* network;
	sf::Clock clock;
	sf::Time acc = sf::Time::Zero;
	sf::Time ups = sf::seconds(1.f / 144.f);
	sf::Clock start;
	float timeAlive;

	void LoadSpeed();
	void SetDead();
	int LoadTexture();
	int LoadTexture(std::string name);
	void AdvanceLap();
	void BorderCollision();
	void FindLines();
	inline bool LineIntersection(sf::Vector2f&, sf::Vector2f&, sf::Vector2f&, sf::Vector2f&);
	inline sf::Vertex FindLine(TrackInfo::RGB&, sf::Image&, std::function<sf::Vector2<int>(const sf::FloatRect&, const sf::Transform&, int)>);
	inline int GetLength(sf::Vertex&, sf::Vertex&);
	inline bool ValidColour(const sf::Color&);
	inline bool ValidColour(sf::Color&);
	inline void UpdateRect();
	void UpdatePoints();
	void CheckStuck();
	void FindMove();
	void WaypointCollision();
	std::array<int, 5> GetLineLengths();
	inline bool WithinTolerance(sf::Vector2f&, sf::Vector2f&, float tolerance);
	inline bool WithinToleranceOfSprite(sf::Vector2f&, float tolerance);
	void Move(Direction);
	Car() = delete;
public:
	struct Laps
	{
		std::vector<std::vector<sf::Vertex>> waypointsPassed;
		unsigned int lap;
		Laps() : lap(0) { /* Empty */ };
	} laps;

	Car(Configuration* config, unsigned int, TrackInfo&, sf::Image&, std::vector<std::vector<sf::Vertex>>&, Network*);
	Car(const Car& c);

	Network* GetNetwork();
	void Run();
	void Reset();

	sf::Sprite GetSprite();
	unsigned int GetScore();
	unsigned int GetID();
	bool IsDead();
	float GetTimeAlive();
	std::array<sf::Vertex[2], 5>& GetLines();

	bool operator==(Car& rhs) { return id == rhs.id; }
	bool operator==(const Car& rhs) const { return id == rhs.id; }
	bool operator==(Car* rhs) { return *this == rhs; }
	bool operator==(const Car* rhs) const { return *this == rhs; }
	bool operator!=(Car& rhs) { return !(*this == rhs); }
	bool operator!=(const Car& rhs) const { return !(*this == rhs); }
	bool operator!=(Car* rhs) { return !(*this == *rhs); }
	bool operator!=(const Car* rhs) const { return !(*this == *rhs); }
};

#endif