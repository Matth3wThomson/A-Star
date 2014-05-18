#pragma once

#include "HexGrid.h"
#include <iostream>
#include <string>
#include <sfml\System.hpp>
#include <sfml\Window.hpp>
#include <sfml\OpenGL.hpp>
#include <sfml\Graphics.hpp>

using std::vector;
using std::priority_queue;
using std::list;
using std::map;

int main(void)
{
	sf::RenderWindow window(sf::VideoMode(1280, 720), "HEX GRID");

	HexGrid hg("HexMap.txt");
	//std::cout << hg << std::endl;
	std::list<HexGrid::HexNode*> traversed;

	int mode = 0;

	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed){
				window.close();
			}
			if (event.type == sf::Event::KeyPressed){
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::M)){
					mode++;
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return)){
					std::cout << "Please enter the identity of the node you wish" <<
						" to set as your city. \n";
					int x = 0;
					std::cin >> x;

					int y = hg.AStar(x);
					std::cout << "Cost of travel to " << x << " is: " << y << std::endl;
					std::cin >> x;
				}
			}
		}

		window.clear(sf::Color(50, 50, 50, 255));

		hg.Draw(window, 300, 650, mode);

		for (list<HexGrid::HexNode*>::const_iterator i = traversed.begin();
			i != traversed.end(); ++i){

		}

		window.display();
	}
	return 0;
}

//Cost of given node, "f value", f = g + h
// g = cost of node - absolute

//h = heuristic "best guess" of the minimum cost left
//to travel between considered node and goal - best guess
