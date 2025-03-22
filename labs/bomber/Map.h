#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <algorithm>
#include <queue>
#include <vector>
#include <cmath>
#include "Point.h"

struct hashIndex{
        size_t operator()(const Point& point) const{
                return pow(point.lat, point.lng+1);
        }
};

struct State{
        Point position;
        int bombCount;
        char lastMove;

	State(): position(Point(-1,-1)), bombCount(0), lastMove(' '){}

        State(Point posi, int bC, char last = ' ') : position(posi), bombCount(bC), lastMove(last){}
};

struct hashState{
	size_t operator()(const State& s) const{
                return pow((s.position).lat, (s.position).lng+1) * s.bombCount;
        }
};

struct moveWithDirection{
	Point pos;
	char direct;

	moveWithDirection(Point posi, char directi) : pos(posi), direct(directi){}
};

struct stateWithScore{
	State s;
	int score;

	stateWithScore(State sta, int sc) : s(sta), score(sc){};
};

struct compareScores{
	bool operator()(const stateWithScore& current, const stateWithScore other) const{
		return current.score > other.score;
	}
};

class Map {
	std::unordered_map<Point, std::vector<moveWithDirection>, hashIndex> adjacent;
	std::vector<std::string> stringMap;
	int length = 0;
	int width = 0;
	std::vector<Point> bombCords; 
    	std::vector<Point> brokenWallCords;
	std::map<char, Point> directions = {{'E', Point(0, 1)}, {'W', Point(0, -1)}, {'S', Point(1,0)}, {'N', Point(-1, 0)}}; 
	int numberOfBombs = 0;
	bool walkable(int lati, int longi);
	void addEdge(Point key, Point neighbor, char d);
	std::vector<moveWithDirection> getNeighbors(Point current);
	int heuristic(const Point& current, const Point& goal);
	void revertChanges();
	std::string displayPath(const std::unordered_map<State, State, hashState> previous, State current);
public:
    Map(std::istream& stream);
    // ~Map();

    std::string route(Point src, Point dst);
};

#endif
