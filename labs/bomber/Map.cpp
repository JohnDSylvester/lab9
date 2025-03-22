#include "Map.h"
#include "Errors.h"

// Member Functions

bool operator== (const State& other, const State& current){
	return current.position == other.position && current.bombCount == other.bombCount;
}


void Map::addEdge(Point key, Point neighbor, char d){
	adjacent[key].push_back(moveWithDirection(neighbor, d));
}

bool Map::walkable(int lati, int longi){
	return lati >= 0 && longi >= 0 && lati < length && longi < width && stringMap[lati][longi] != '~';
}

std::vector<moveWithDirection> Map::getNeighbors(Point current){
	return adjacent[current];
}

Map::Map(std::istream& stream){
	std::string line = "";
	while(std::getline(stream, line)){
		stringMap.push_back(line);
		length++;
	}
	if(length != 0){
		width = stringMap[0].size();
	}

	for(int y = 0; y < length; y++){
		for(int x = 0; x < width; x++){
			Point current(y, x);
			if(stringMap[y][x] == '~'){
				continue;
			}
			for(auto& [compass, move]: directions){
				int newX = x + move.lng;
				int newY = y + move.lat;
							
				if(walkable(newY, newX)){
					addEdge(current,Point(newY, newX), compass);
				}
			}
		}
	}
}

std::string Map::route(Point src, Point dst){
	if(src.lat >= length || src.lng >= width || src.lat < 0 || src.lng < 0 || stringMap[src.lat][src.lng] == '#' || stringMap[src.lat][src.lng] == '~'){
                throw(PointError(src));
	}
	else if(dst.lat >= length || dst.lng >= width || dst.lat < 0 || dst.lng < 0){
                throw(PointError(dst));
        }

	std::priority_queue<stateWithScore, std::vector<stateWithScore>, compareScores> openSet;
	std::unordered_map<State, State, hashState> previous;
	std::unordered_map<State, int, hashState> gScore;
	std::unordered_map<State, int, hashState> fScore;
	std::unordered_set<State, hashState> visited;

	openSet.push(stateWithScore(State(src, 0), 0));
	gScore[State(src, 0)] = 0;
	fScore[State(src, 0)] = heuristic(src, dst);

	while(!openSet.empty()) {
		State current = openSet.top().s;
		openSet.pop();

		if(current.position == dst){
			return displayPath(previous, current);
		}

		if(visited.count(current)){
			continue;
		}
		visited.insert(current);

		std::vector<moveWithDirection> neighbors = getNeighbors(current.position);

		for(const moveWithDirection& neighbor : neighbors){
			Point neighborPoint = neighbor.pos;
			char direction = neighbor.direct;
			char space = stringMap[neighborPoint.lat][neighborPoint.lng];
			int bombs = current.bombCount;

			if(space == '#'){
				if(bombs == 0){
					continue;
				}
				else if(bombs > 0){
					bombs--;
					stringMap[neighborPoint.lat][neighborPoint.lng] = '.';
					brokenWallCords.push_back(neighborPoint);
				}
			}
			else if(space == '*'){
				bombs++;
                                stringMap[neighborPoint.lat][neighborPoint.lng] = '.';
                                bombCords.push_back(neighborPoint);
			}

			State nextState(neighborPoint, bombs, direction);
			

			int maybeGScore = gScore[current] + 1;

			if(!gScore.count(nextState) || maybeGScore < gScore[nextState]){
				previous[nextState] = current;
				gScore[nextState] = maybeGScore;
				fScore[nextState] = maybeGScore + heuristic(neighborPoint, dst);
				openSet.push(stateWithScore(State(neighborPoint, bombs, direction), fScore[nextState]));
			}
		} 
		revertChanges();
	}
	
	throw(RouteError(src, dst));
}

void Map::revertChanges(){
	for(Point& bomb : bombCords){
		stringMap[bomb.lat][bomb.lng] = '*';
	} 

	bombCords.clear();

	for(Point& wall : brokenWallCords){
                stringMap[wall.lat][wall.lng] = '#';
        }

        brokenWallCords.clear();
}

std::string Map::displayPath(const std::unordered_map<State, State, hashState> previous, State current){
	std::string path = "";
	while(previous.find(current) != previous.end()){
		if(current.lastMove != ' '){
		path += current.lastMove;
		}
		current = previous.at(current);
	}
	std::reverse(path.begin(), path.end());
	revertChanges();
	return path;
}

int Map::heuristic(const Point& current, const Point& goal){
	return abs(current.lat - goal.lat) + abs(current.lng - current.lng);
}
