#pragma once

#include <vector>
#include <map>
#include <queue>
#include <list>
#include <set>
#include <unordered_set>

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <sfml\Graphics.hpp>

#include "Common.h"

using std::map;
using std::vector;

using std::string;
using std::ifstream;
using std::ostream;

static const float radius = 20.0f;

class HexGrid
{
public:

	//References to other nodes is not necessary.
	//Position infers that information
	struct HexNode {
		const int identity;
		int x, y;
		HexNode() : identity(id++){ }
	};

	//Less than 1 is impassable, -1 and -2 are special values
	enum nodeType {
		START = -1, WATER = 0, PLAIN = 1, DUNE = 2, FOREST = 4
	};

	HexGrid(void);
	HexGrid(const int& x, const int& y);
	HexGrid(const string& filename);
	~HexGrid(void);

	void CreateNode(const int& x, const int& y, const nodeType& type);
	
	//Set the type of a node
	void SetNodeType(const HexNode& node, const nodeType& type){
		gValues.erase(node.identity);
		gValues.insert(std::pair<int, nodeType>(node.identity, type));
	}

	//Set type by node id
	void SetNodeType(const int& id, const nodeType& type){
		gValues.erase(id);
		gValues.insert(std::pair<int, nodeType>(id, type));
	}

	//Set type by coordinate
	void SetNodeType(const int&x, const int& y, const nodeType& type){
		gValues.erase(nodes[x][y]->identity);
		gValues.insert(std::pair<int, nodeType>(nodes[x][y]->identity, type));
	}

	void Draw(sf::RenderWindow& w, const float& x, const float& y, int mode);

	inline int Distance(const HexNode& from, const HexNode& to) const{
		//max(x2-x1, y2-y1, z2-z1)
		int x = max(abs(to.x - from.x), abs(to.y - from.y));
		return max(x , abs(CalculateNodeZ(to) - CalculateNodeZ(from)));
	}

	inline int CalculateNodeZ(const HexNode& node) const {
		//x + y + z = 0
		return -(node.x + node.y);
	}

	int AStar(const int& nodeID);

	friend ostream& operator<<(ostream& os, const HexGrid& hg){
		hg.Print(os);
		return os;
	}

	void DrawTraversed(const HexNode& node, sf::RenderWindow& window, const float& x, const float& y);

protected:

	struct AStarNode {
		int x, y, f, g, h, id, cameFrom;

		AStarNode(const int& x, const int& y, const int& g, const int& h, const int& id){
			this->x = x;
			this->y = y;
			this->g = g;
			this->h = h;
			this->f = g + h;
			this->id = id;
			cameFrom = -1;
		}

		bool operator<(const AStarNode& rhs) const{
			if (this->f > rhs.f){
				return true;
			}
			return false;
		}

		bool operator==(const AStarNode& rhs) const{
			if ( (this->x == rhs.x) && (this->y == rhs.y) ){
				return true;
			}
			return false;
		}

		bool operator==(const AStarNode& rhs){
			if ( (this->x == rhs.x) && (this->y == rhs.y) ){
				return true;
			}
			return false;
		}
	};

	//For creating unique ids for nodes
	static int id;

	//2d Vector of nodes
	vector<vector<HexNode*>> nodes;

	//Map of node id to node type
	map<int, nodeType> gValues;

	void LoadMap(const string& filename);
	ostream& Print(ostream& o) const;
	ostream& PrintHexNode(ostream& o, const HexNode& hn) const;
	HexNode* FindNodeByID(const int& id) const;

	sf::Font* font;
	sf::Text* text;
};



