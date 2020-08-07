#ifndef _TRACK_INFO_H
#define _TRACK_INFO_H

#include <SFML/Graphics.hpp>

class TrackInfo
{
private:
	void floodFill(sf::Image&, std::vector<sf::Vector2<int>>&, sf::Vector2<int>);
	inline float getLength(sf::Vector2<int>&, sf::Vector2<int>&);
public:
	struct RoadRGB {
		int r, g, b;
		inline bool operator==(const sf::Color& rhs) { return ((int)rhs.r == r && (int)rhs.g == g && (int)rhs.b == b); }
		inline bool operator!=(const sf::Color& rhs) { return !(*this == rhs); }
	} roadColour;
	struct LineRGB {
		int r, g, b;
		inline bool operator==(const sf::Color& rhs) { return ((int)rhs.r == r && (int)rhs.g == g && (int)rhs.b == b); }
		inline bool operator!=(const sf::Color& rhs) { return !(*this == rhs); }
	} lineColour;
	struct WaypointRGB {
		int r, g, b;
		inline bool operator==(const sf::Color& rhs) { return ((int)rhs.r == r && (int)rhs.g == g && (int)rhs.b == b); }
		inline bool operator!=(const sf::Color& rhs) { return !(*this == rhs); }
	} waypointColour;
	struct Bounds {
		sf::Vector2<int> top;
		sf::Vector2<int> bottom;
		sf::Vector2<int> left;
		sf::Vector2<int> right;
		Bounds()
		{
			top		= { 0,0 };
			bottom	= { INT_MAX, INT_MAX };
			left	= { INT_MAX, INT_MAX };
			right	= { 0,0 };
		}
		// Inclusive bounds check
		inline bool contains(sf::Vector2<int> point)
		{
			return (
				point.y <= top.y && point.y >= bottom.y &&
				point.x <= right.x && point.x >= left.x
				);
		}
	};

	float scaleX, scaleY;
	float posX, posY;
	float rotation;
	std::vector<Bounds> bounds;
	std::vector<std::vector<sf::Vertex>> waypoints;

	// We need to find the bounding boxes for each of the waypoints on the track
	std::vector<std::vector<sf::Vertex>> findWaypoints(sf::Image&);
	int loadTrackInfo(std::string);

};

#endif // !_TRACK_INFO_H
