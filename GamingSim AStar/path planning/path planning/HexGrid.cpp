#pragma once

#include "HexGrid.h"

int HexGrid::id = 0;

HexGrid::HexGrid(void){ 
	font = new sf::Font();

	if (!font->loadFromFile("ARIAL.ttf")){
		exit(13);
	}

	text = new sf::Text();
	text->setFont(*font);
};

HexGrid::HexGrid(const int& x, const int& y){
	//Declare a vector containing x number of vectors of length y.
	nodes = vector<vector<HexNode*>>(x, std::vector<HexNode*>(y));

	font = new sf::Font();

	if (!font->loadFromFile("ARIAL.ttf")){
		exit(13);
	}

	text = new sf::Text();
	text->setFont(*font);
}

HexGrid::HexGrid(const string& filename){
	LoadMap(filename);

	font = new sf::Font();

	if (!font->loadFromFile("ARIAL.ttf")){
		exit(13);
	}

	text = new sf::Text();
	text->setFont(*font);
}

HexGrid::~HexGrid(){

}


void HexGrid::CreateNode(const int& x, const int& y, const nodeType& type){
	//Create the new node
	HexNode* b = new HexNode();

	//Delete the node currently in that nodes position (if there is one)
	if (nodes[x][y] != NULL){ 
		//First remove their identity from the mapping
		gValues.erase(nodes[x][y]->identity);

		//Then delete the dynamic memory
		delete nodes[x][y];

		//Then remove the pointer from the 2d vector
		nodes[x].erase(nodes[x].begin()+y); 
	}

	//Then add our new node
	nodes[x][y] = b;

	//Make the node aware of its position
	b->x = x;
	b->y = y;

	//And add its fValue to the mapping
	gValues.insert(std::pair<int, nodeType>(b->identity, type));
}


void HexGrid::LoadMap(const string& filename){
	ifstream f(filename);

	if (!f){
		return;
	}

	int x, y, z;
	f >> x;
	f >> y;

	//Set up the hexGrid container
	nodes = vector<vector<HexNode*>>(x, std::vector<HexNode*>(y));

	while (!f.eof()){
		f >> x;
		f >> y;
		f >> z;

		//TODO: Test this cast
		CreateNode(x, y, (nodeType) z);
	}
}

ostream& HexGrid::Print(ostream& os) const{
	//Loop the values of the y THIS PRINTS Y BY X (Normal graphing)
	for (int i= nodes[0].size()-1;  i > -1; --i){
		os << " ";
		//Check all values of x
		for (vector<vector<HexNode*>>::const_iterator j = nodes.begin(); j != nodes.end(); ++j){
			if ((*j)[i] != NULL){
				os << (*j)[i]->identity << " ";
			} else {
				os << " ";
			}
		}
		os << " \n";
	}
	return os;
}

void HexGrid::DrawTraversed(const HexNode& node, sf::RenderWindow& window, const float& x, const float& y){
	sf::CircleShape marker(radius / 2, 15);
	marker.setFillColor(sf::Color(255, 51, 51));

	float x2 = x + node.x*2*radius + node.y*radius;
	float y2 = y + -node.y*2*radius;

	marker.setPosition(x2, y2);
	window.draw(marker);
};

void HexGrid::Draw(sf::RenderWindow& window, const float& x, const float& y, int mode){
	sf::CircleShape hex(radius, 6);
	text->setCharacterSize((int) radius);

	mode = mode%3;

	text->setPosition(x, y);
	text->setColor(sf::Color::White);

	if (mode == 1){
		text->setString("IDs");
		window.draw(*text);
	} else if (mode == 2){
		text->setString("COORDS");
		window.draw(*text);
	}

	for (int i= nodes[0].size()-1;  i > -1; --i){
		//Check all values of x
		for (vector<vector<HexNode*>>::const_iterator j = nodes.begin(); j != nodes.end(); ++j){
			if ((*j)[i] != NULL){

				std::ostringstream o;

				switch (mode){
				case 0:
					//Dont display any text on the nodes
					o << "";
					break;
				case 1:
					//Display the node Ids
					o << (*j)[i]->identity;
					break;
				case 2:
					//Display their coordinates
					o << (*j)[i]->x << "," << (*j)[i]->y;
					break;
				}
				text->setString(o.str());

				float x2 = x + (*j)[i]->x*2*radius + (*j)[i]->y*radius;
				float y2 = y + -(*j)[i]->y*2*radius;

				hex.setPosition(x2, y2);
				text->setPosition(x2, y2 + 0.5f * radius);
				text->setColor(sf::Color::White);
				
				switch (gValues.at((*j)[i]->identity)){
				case -1:
					//RED start
					hex.setFillColor(sf::Color(255, 51, 51));
					text->setColor(sf::Color(0, 204, 204));
					break;
				case 0:
					//BLUE water
					hex.setFillColor(sf::Color(51, 153, 255));
					text->setColor(sf::Color(204, 102, 0));
					break;
				case 1:
					//LIGHT GREEN plains
					hex.setFillColor(sf::Color(153, 255, 51));
					text->setColor(sf::Color(102, 0, 204));
					break;
				case 2:
					//YELLOW dunes
					hex.setFillColor(sf::Color(255, 255, 51));
					text->setColor(sf::Color(0, 0, 204));
					break;
				case 4:
					//DARK GREEN forest
					hex.setFillColor(sf::Color(76, 153, 0));
					text->setColor(sf::Color::White);
					break;
				}
				window.draw(hex);
				window.draw(*text);
			}
		}
	}
}

int HexGrid::AStar(const int& nodeID){
	std::set<int> closedList;
	std::priority_queue<AStarNode> openList;
	std::list<HexNode*> toReturn;

	//Quickest way to iterate through a nodes connected nodes
	std::pair<int, int> connectedNodes[6] = { 
		std::pair<int, int>(0,1),
		std::pair<int, int>(1,0),
		std::pair<int, int>(0,-1),
		std::pair<int, int>(-1,0),
		std::pair<int, int>(-1,1),
		std::pair<int, int>(1,-1)
	};

	//Find the goal node given its ID
	HexNode goalNode = *FindNodeByID(nodeID);

	//Create an AStarNode from the goalNode
	AStarNode goal(goalNode.x, goalNode.y, gValues.at(goalNode.identity), 0, goalNode.identity);

	//Find the start node
	int startID = 0;
	for (map<int, nodeType>::const_iterator i = gValues.begin(); i != gValues.end(); ++i){
		if (i->second == START){ startID = i->first; break; }
	}

	HexNode startNode = *FindNodeByID(startID);
	
	//Assign f values to A, and add to open list.
	AStarNode A(startNode.x, startNode.y, 0, Distance(startNode, goalNode), startNode.identity);
	openList.push(A);

	//A Path cannot be found
	while (!openList.empty()){

		AStarNode P = openList.top();
		openList.pop();

		std::cout << "\n P is now: (" << P.x << "," << P.y << ")\n";

		if (P.id == goal.id){
			return P.g;
		}

		//Remove from openlist, and add to closed list
		closedList.insert(P.id);

		//For all nodes directly connected to P
		for (int i = 0; i < 6; ++i){
			//Obtain a pointer to that node
			int x = P.x + connectedNodes[i].first;
			int y = P.y + connectedNodes[i].second;

			//Requested node is out of range of the 2d Array
			if (x < 0 || x >= nodes.size()){
				continue;
			}

			//Requested node is out of range of the 2D Array
			if (y < 0 || y >= nodes[x].size()){
				continue;
			}

			HexNode* Q = nodes[x][y];

			std::cout << "Testing coordinates (" << x << "," << y << ")\n";

			if (Q == NULL){ std::cout << "NO NODE\n"; continue; }//There is no node at that location

			//Calculate f, g and h values for Q
			AStarNode Qst(Q->x, Q->y, gValues.at(Q->identity), Distance(*Q, goalNode), Q->identity);

			//The node is on the closed list, or is water
			if (closedList.find(Qst.id) != closedList.end() || Qst.g < 1){
				if (Qst.g < 1){
					std::cout << "G VALUE LOWER: " << Qst.g << std::endl;
				} else {
					std::cout<< "IN CLOSED LIST\n";
				}
				continue; 
			}

			//We always add the node to the open list (Closed list will prevent duplicates)
			int gScore = P.g + gValues.at(Q->identity);

			Qst.cameFrom = nodes[P.x][P.y]->identity;
			Qst.g = gScore;
			Qst.f = gScore + Distance(*Q, goalNode);

			std::cout << "SUCCEED\n";
			openList.push(Qst);
		}
	}

	//Path finding failed.
	std::cout << "FAILED!?";
	return -1;
}

HexGrid::HexNode* HexGrid::FindNodeByID(const int& id) const {
	//n^2 loop through 2d Array
	for (vector<vector<HexNode*>>::const_iterator i = nodes.begin(); i != nodes.end(); ++i){
		for (vector<HexNode*>::const_iterator j = i->begin(); j != i->end(); ++j){
			if ((*j) == NULL) continue;

			if ((*j)->identity == id){
				return *j;
			}
		}
	}
	return NULL;
}