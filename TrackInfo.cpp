#include <fstream>
#include <sstream>

#include "TrackInfo.h"

void TrackInfo::floodFill(sf::Image& track, std::vector<sf::Vector2<int>>& checked, sf::Vector2<int> point)
{
	// Keep track of cells we have checked
	int count = std::count_if(
		checked.begin(),
		checked.end(),
		[&](sf::Vector2<int> v) {
			return v == point;
		}
	);

	if (count > 0)
		return;

	// This is a new point
	checked.push_back(point);

	// Bounds check
	if (point.x < 0 || point.x >= track.getSize().x ||
		point.y < 0 || point.y >= track.getSize().y)
		return;

	if (waypointColour != track.getPixel(point.x, point.y))
		return;

	// This pixel matches the colour
	// Check for bounds
	if (point.x < bounds[bounds.size() - 1].left.x)
		bounds[bounds.size() - 1].left = point;
	if (point.x > bounds[bounds.size() - 1].right.x)
		bounds[bounds.size() - 1].right = point;
	if (point.y < bounds[bounds.size() - 1].bottom.y)
		bounds[bounds.size() - 1].bottom = point;
	if (point.y > bounds[bounds.size() - 1].top.y)
		bounds[bounds.size() - 1].top = point;


	// Let's look at the surrounding pixels
	floodFill(
		track,
		checked,
		sf::Vector2<int>({ point.x + 1, point.y })
	);

	floodFill(
		track,
		checked,
		sf::Vector2<int>({ point.x - 1, point.y })
	);

	floodFill(
		track,
		checked,
		sf::Vector2<int>({ point.x, point.y + 1 })
	);

	floodFill(
		track,
		checked,
		sf::Vector2<int>({ point.x, point.y - 1 })
	);
}

inline float TrackInfo::getLength(sf::Vector2<int>& a, sf::Vector2<int>& b)
{
	return std::sqrt(
		(b.x - a.x) * (b.x - a.x) +
		(b.y - a.y) * (b.y - a.y)
	);
}

// We need to find the bounding boxes for each of the waypoints on the track
std::vector<std::vector<sf::Vertex>> TrackInfo::findWaypoints(sf::Image& track)
{
	// Loop over each pixel in the track and find where the waypoints are
	for (int x = 0; x < track.getSize().x; ++x)
	{
		for (int y = 0; y < track.getSize().y; ++y)
		{
			if (waypointColour == track.getPixel(x, y))
			{
				// We found a waypoint - let's check it's new
				bool found = false;
				for (std::vector<TrackInfo::Bounds>::iterator b = bounds.begin(); b != bounds.end(); ++b)
					if (b->contains({ x, y }))
						found = true;

				if (!found)
				{
					// We found a new point!
					// Let's find our bounds

					// Expand to next waypoint
					bounds.resize(bounds.size() + 1);

					std::vector<sf::Vector2<int>> checked;

					floodFill(
						track,
						checked,
						{ x, y }
					);

					std::vector<sf::Vector2<int>> points = {
						bounds[bounds.size() - 1].left,
						bounds[bounds.size() - 1].bottom,
						bounds[bounds.size() - 1].top,
						bounds[bounds.size() - 1].right
					};

					// Find the longest distance between two points
					float largest = -1.0;
					std::vector<sf::Vertex> line(2);
					for (int outer = 0; outer < points.size(); ++outer)
						for (int inner = points.size() - 1; inner > outer; --inner)
						{
							const float len = getLength(points[outer], points[inner]);
							if (largest < len) {
								line[0] = sf::Vertex({ (float)points[outer].x, (float)points[outer].y });
								line[1] = sf::Vertex({ (float)points[inner].x, (float)points[inner].y });

								largest = len;
							}
						}

					waypoints.push_back(line);
				}
			}
		}
	}

	return waypoints;
}

//FIXME: Make an readable file format + parser
int TrackInfo::loadTrackInfo(std::string track)
{
	std::ifstream infile(track);
	if (!infile.good())
		return EXIT_FAILURE;

	std::string line;

	std::getline(infile, line);
	posX = std::stof(line);
	std::getline(infile, line);
	posY = std::stof(line);

	std::getline(infile, line);
	scaleX = std::stof(line);
	std::getline(infile, line);
	scaleY = std::stof(line);

	std::getline(infile, line);
	rotation = std::stof(line);

	std::getline(infile, line);
	roadColour.r = std::stoi(line);
	std::getline(infile, line);
	roadColour.g = std::stoi(line);
	std::getline(infile, line);
	roadColour.b = std::stoi(line);

	std::getline(infile, line);
	lineColour.r = std::stoi(line);
	std::getline(infile, line);
	lineColour.g = std::stoi(line);
	std::getline(infile, line);
	lineColour.b = std::stoi(line);

	std::getline(infile, line);
	waypointColour.r = std::stoi(line);
	std::getline(infile, line);
	waypointColour.g = std::stoi(line);
	std::getline(infile, line);
	waypointColour.b = std::stoi(line);

	infile.close();

	return EXIT_SUCCESS;
}
