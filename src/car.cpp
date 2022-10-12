#include <vector>
#include <string>
#include <filesystem>
#include <assert.h>
#include <random>
#include <stdlib.h>
#include <cstddef>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "car.hpp"
#include "trackinfo.hpp"

Car::Car(Configuration* config, unsigned int id, TrackInfo& trackInfo, sf::Image& track, std::vector<std::vector<sf::Vertex>>& waypoints, Network* network)
	: config(config), id(id), trackInfo(trackInfo), track(track), waypoints(waypoints), network(network)
{
	sprite.setPosition(trackInfo.posX, trackInfo.posY);
	sprite.setScale(trackInfo.scaleX, trackInfo.scaleY);
	sprite.setRotation(trackInfo.rotation);

	// Let's set a texture for this car
	assert(LoadTexture() == EXIT_SUCCESS);

	// Set the origin to the centre for rotation
	sprite.setOrigin(sf::Vector2f(sprite.getTexture()->getSize().x * 0.5f, sprite.getTexture()->getSize().y * 0.5f));

	previousBounds = sprite.getLocalBounds();
	previousPos = sprite.getPosition();

	LoadSpeed();

	UpdatePoints();
	FindLines();
}

Car::Car(const Car& c) : config(c.config), trackInfo(c.trackInfo), track(c.track)
{ 
	network = c.network;
	id = c.id;
	waypoints = c.waypoints;
	trackInfo = c.trackInfo;
	track = c.track;
	texture = c.texture;
	sprite = c.sprite;
	LoadSpeed();
}

void Car::LoadSpeed()
{
	moveSpeed = config->GetCarMoveSpeed();
	rotateSpeedRatio = config->GetCarRotateSpeed();
	maxSpeed = moveSpeed * config->GetCarMaxSpeedMultiplier();
}

void Car::Reset()
{
	sprite.setPosition(trackInfo.posX, trackInfo.posY);
	sprite.setRotation(trackInfo.rotation);

	previousPos = sprite.getPosition();

	UpdateRect();
	UpdatePoints();
	FindLines();

	// Revert Alpha
	sf::Color current = sprite.getColor();
	current.a = 255;
	sprite.setColor(current);

	laps.waypointsPassed.clear();
	laps.lap = 0;

	moveCount = 0;

	start.restart();

	// Bring the car back to life
	dead = false;
}

void Car::WaypointCollision()
{	
	// Check if we have advanced a lap
	if (laps.waypointsPassed.size() == waypoints.size())
	{
		// We have passed all waypoints - check we have crossed the finish line
		AdvanceLap();
		return;
	}

	size_t startSize = laps.waypointsPassed.size();

	// Treat each edge of the car sprite as a line - check for line collision with it
	for (std::vector<std::vector<sf::Vertex>>::iterator waypoint = waypoints.begin(); waypoint != waypoints.end(); ++waypoint)
	{
		std::vector<std::vector<sf::Vertex>>::iterator find = std::find_if(
			laps.waypointsPassed.begin(),
			laps.waypointsPassed.end(),
			[&](std::vector<sf::Vertex> v) {
				return (waypoint->at(0).position == v.at(0).position && waypoint->at(1).position == v.at(1).position);
			}
		);

		if (find != laps.waypointsPassed.end())
			continue;

		// Top
		if (LineIntersection(waypoint->at(0).position, waypoint->at(1).position, points[0], points[2]))
		{
			laps.waypointsPassed.push_back(*waypoint);
			break;
		}
		// Left
		if (LineIntersection(waypoint->at(0).position, waypoint->at(1).position, points[0], points[3]))
		{
			laps.waypointsPassed.push_back(*waypoint);
			break;
		}
		// Right
		if (LineIntersection(waypoint->at(0).position, waypoint->at(1).position, points[2], points[5]))
		{
			laps.waypointsPassed.push_back(*waypoint);
			break;
		}
		// Bottom
		if (LineIntersection(waypoint->at(0).position, waypoint->at(1).position, points[3], points[5]))
		{
			laps.waypointsPassed.push_back(*waypoint);
			break;
		}
	}
}

sf::Sprite Car::GetSprite()
{
	return sprite;
}

unsigned int Car::GetScore()
{
	return (laps.lap + 1) * (laps.waypointsPassed.size() + 1);
}

unsigned int Car::GetID()
{
	return id;
}

Network* Car::GetNetwork()
{
	return network;
}

bool Car::IsDead()
{
	return dead;
}

float Car::GetTimeAlive()
{
	return timeAlive;
}

std::array<sf::Vertex[2], 5>& Car::GetLines()
{
	return lines;
}

int Car::LoadTexture()
{
	std::vector<std::string> images;
	for (auto& file : std::filesystem::directory_iterator("./images"))
		images.push_back((file.path().string()));

	std::random_device dev;
	std::uniform_int_distribution<int> dist(0, images.size() - 1);
	std::mt19937 mt(dev());

	texture = new sf::Texture();
	if (!texture->loadFromFile(images[dist(mt)]))
		return EXIT_FAILURE;

	sprite.setTexture(*texture);

	return EXIT_SUCCESS;
}

inline sf::Vertex Car::FindLine(TrackInfo::RGB& roadColour, sf::Image& track,
	std::function<sf::Vector2<int>(const sf::FloatRect&, const sf::Transform&, int)> update)
{
	const int MAX_MARCH = 1000;
	const int ACCURACY_THRESHOLD = MAX_MARCH / 50;

	unsigned int march = 0;
	sf::Vector2<int> point;
	do
	{
		point = update(rect, trans, march);
		if (march >= MAX_MARCH)
			break;
		// Get less accurate the larger march is
		if (march >= ACCURACY_THRESHOLD)
			march *= 1.5;
		else ++march;
		// Check bounds
		if (point.x < 0) point.x = 0;
		if (point.x >= track.getSize().x) point.x = track.getSize().x - 1;
		if (point.y < 0) point.y = 0;
		if (point.y >= track.getSize().y) point.y = track.getSize().y - 1;
	} while (ValidColour(track.getPixel(point.x, point.y)));

	return sf::Vertex({ (float)point.x, (float)point.y });
}

inline int Car::GetLength(sf::Vertex& a, sf::Vertex& b)
{
	return std::sqrt(
		(b.position.x - a.position.x) * (b.position.x - a.position.x) +
		(b.position.y - a.position.y) * (b.position.y - a.position.y)
	);
}

void Car::SetDead()
{
	// The car has crashed!
	// Change state to dead
	dead = true;

	// Record time alive
	timeAlive = start.getElapsedTime().asSeconds();

	sf::Color current = sprite.getColor();
	current.a = 100;
	sprite.setColor(current);
}

inline bool Car::ValidColour(const sf::Color& pixel)
{
	return trackInfo.roadColour == pixel || trackInfo.lineColour == pixel || trackInfo.waypointColour == pixel;
}

inline void Car::UpdateRect()
{
	rect = sprite.getLocalBounds();
	trans = sprite.getTransform();
}

void Car::UpdatePoints()
{
	// Find the points to detect collision
	// To speed this up we will only look at each vertex
	// and the mid point between each vertex pair

	// Top verticies
	points[0] = trans * sf::Vector2f({ rect.left, rect.top });
	points[1] = trans * sf::Vector2f({ (rect.left + rect.width) / 2, rect.top });
	points[2] = trans * sf::Vector2f({ rect.left + rect.width, rect.top });

	// Bottom verticies
	points[3] = trans * sf::Vector2f({ rect.left, rect.top + rect.height });
	points[4] = trans * sf::Vector2f({ (rect.left + rect.width) / 2, rect.top + rect.height });
	points[5] = trans * sf::Vector2f({ rect.left + rect.width, rect.top + rect.height });

	// Middle verticies
	points[6] = trans * sf::Vector2f({ rect.left, (rect.top + rect.height) / 2 });
	points[7] = trans * sf::Vector2f({ rect.left + rect.width, (rect.top + rect.height) / 2 });
}

void Car::CheckStuck()
{
	// This function combats cars spamming LEFT & RIGHT or FORWARDS & BACKWARDS
	//if (WithinToleranceOfSprite(previousPos, moveSpeed * 2.f))
		//SetDead();

	// Check the car is making progress
	if (moveCount >= 1000)
	{
		moveCount = 0;

		if (GetScore() == lastScore)
			SetDead();

		lastScore = GetScore();
	}
		
	previousPos = sprite.getPosition();
}

inline bool Car::WithinTolerance(sf::Vector2f& v1, sf::Vector2f& v2, float tolerance)
{
	sf::Vector2f res = v2 - v1;
	return (abs(res.x) < tolerance && abs(res.y) < tolerance);
}

inline bool Car::WithinToleranceOfSprite(sf::Vector2f& v1, float tolerance)
{
	sf::Vector2f res = sprite.getPosition() - v1;
	return (abs(res.x) < tolerance && abs(res.y) < tolerance);
}

void Car::FindLines()
{

	// Check if the car has moved, no need to re-compute if not
	if (rect == previousBounds)
		return;

	lines[0][0] = sf::Vertex({ trans * sf::Vector2f({ (rect.left + rect.width) / 2.f, rect.top }) });
	lines[0][1] = FindLine(
		trackInfo.roadColour,
		track,
		[](const sf::FloatRect& rect, const sf::Transform& trans, int march) -> sf::Vector2<int> {
			return (sf::Vector2<int>)(trans * sf::Vector2f((rect.left + rect.width) / 2.f, rect.top - march));
		}
	);


	lines[1][0] = sf::Vertex({ trans * sf::Vector2f({ rect.left + rect.width, rect.top }) });
	lines[1][1] = FindLine(
		trackInfo.roadColour,
		track,
		[](const sf::FloatRect& rect, const sf::Transform& trans, int march) -> sf::Vector2<int> {
			return (sf::Vector2<int>)(trans * sf::Vector2f(rect.width + march / 2.f,  rect.top - march / 2.f)); 
		}
	);

	lines[2][0] = sf::Vertex({ trans * sf::Vector2f({ rect.left, rect.top }) });
	lines[2][1] = FindLine(
		trackInfo.roadColour,
		track,
		[](const sf::FloatRect& rect, const sf::Transform& trans, int march) -> sf::Vector2<int> {
			return (sf::Vector2<int>)(trans * sf::Vector2f(rect.left - march / 2.f,  rect.top - march / 2.f)); 
		}
	);

	lines[3][0] = sf::Vertex({ trans * sf::Vector2f({ rect.left, (rect.top + rect.height) / 2 }) });
	lines[3][1] = FindLine(
		trackInfo.roadColour,
		track,
		[](const sf::FloatRect& rect, const sf::Transform& trans, int march) -> sf::Vector2<int> {
			return (sf::Vector2<int>)(trans * sf::Vector2f(rect.left - march, (rect.left + rect.height) / 2.f));
		}
	);

	lines[4][0] = sf::Vertex({ trans * sf::Vector2f({ rect.left + rect.width, (rect.top + rect.height) / 2.f }) });
	lines[4][1] = FindLine(
		trackInfo.roadColour,
		track,
		[](const sf::FloatRect& rect, const sf::Transform& trans, int march) -> sf::Vector2<int> {
			return (sf::Vector2<int>)(trans * sf::Vector2f(rect.left + rect.width + march, (rect.top + rect.height) / 2.f));
		}
	);
}

std::array<int, 5> Car::GetLineLengths()
{
	return std::array<int, 5>({
			GetLength(lines[0][0], lines[0][1]),
			GetLength(lines[1][0], lines[1][1]),
			GetLength(lines[2][0], lines[2][1]),
			GetLength(lines[3][0], lines[3][1]),
			GetLength(lines[4][0], lines[4][1]),
		});
}

void Car::BorderCollision()
{
	// Check for colour collision on each point
	for (int idx = 0; idx < 8; ++idx)
	{
		const sf::Color pixel = track.getPixel(points[idx].x, points[idx].y);
		if (!ValidColour(pixel))
		{
			SetDead();
			break;
		}
	}
}

// p1 & p2 are the two points that define line A
// p3 & p4 are the two points that define line B
inline bool Car::LineIntersection(sf::Vector2f& p1, sf::Vector2f& p2, sf::Vector2f& p3, sf::Vector2f& p4)
{
	const float denominator = ((p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y));
	const float a = ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / denominator;
	const float b = ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) / denominator;

	return (a >= 0 && a <= 1 && b >= 0 && b <= 1);
}

void Car::AdvanceLap()
{
	if (laps.waypointsPassed.size() != waypoints.size())
		return;

	// Check for colour collision on each point
	for (int idx = 0; idx < 8; ++idx)
	{
		const sf::Color pixel = track.getPixel(points[idx].x, points[idx].y);
		if (trackInfo.lineColour == pixel)
		{
			// We are on the line!
			laps.waypointsPassed.clear();
			++laps.lap;

			return;
		}
	}
}



void Car::Move(Direction dir)
{
	if (dead)
		return;

	UpdateRect();

	if (dir == Direction::FORWARD)
	{
		speed += moveSpeed;
		if (speed > maxSpeed)
			speed = maxSpeed;
	}
	else if (dir == Direction::BACKWARD)
	{
		speed -= moveSpeed;
		if (speed < -maxSpeed)
			speed = -maxSpeed;
	}
	else if (dir == Direction::ROTATE_LEFT)
	{
		sprite.rotate(-speed * rotateSpeedRatio);
	}
	else if (dir == Direction::ROTATE_RIGHT)
	{
		sprite.rotate(speed * rotateSpeedRatio);
	}
		

	if (speed < 0.1f && speed > -0.1f)
		return;

	const float angleRADS = (pi / 180.f) * (sprite.getRotation() + 270.f);
	sprite.move({ static_cast<float>(speed * cos(angleRADS)), static_cast<float>(speed * sin(angleRADS)) });

	// Check we haven't hit anything
	UpdatePoints();

	FindLines();
	BorderCollision();
	WaypointCollision();
}

void Car::FindMove()
{
	std::array<int, 5> lines = GetLineLengths();
	Move(network->PredictMove(lines));

	if (++moveCount % 100 == 0)
		CheckStuck();
}

void Car::Run()
{
	while (acc > ups)
	{
		acc -= ups;
		if (!dead)
			FindMove();
	}
	acc += clock.restart();
}

