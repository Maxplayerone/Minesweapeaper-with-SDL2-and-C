#include<stdio.h>
#include<SDL.h>
#include<stdbool.h>
#include<time.h>
#include<stdlib.h>
#include <Windows.h>

//INITIALIZING THE WINDOW
#define WIDTH 500
#define HEIGHT 500

SDL_Window* window;
SDL_Renderer* renderer;
bool quit = false;

bool Initialize(void);
void Update(float, bool);
void Quit(int);

//COLOURS
typedef struct RGB {
	int r;
	int g;
	int b;
}RGB;

RGB node_color_primary = {255, 255, 255};
RGB node_color_secondary = {200, 200,200};
RGB bomb_color = {0, 0, 0};

//color for every single number of close bombs
RGB node_color = {255, 0, 0};
RGB color_empty = {251, 252, 141};
RGB color_one = {252, 92, 71};
RGB color_two = {237, 131, 50};
RGB color_three = {250, 225, 65};
RGB color_four = {188, 252, 76};
RGB color_five = {54, 209, 27};
RGB color_six = {98, 222, 203};
RGB color_seven = {30, 171, 247};
RGB color_eight = {151, 71, 255};

//RENDERING THE DEEPEST LAYER
void RenderBackNodes(void);

#define NODE_SIZE 25

int row = 25; //the max amount of nodes on one row
int columns = 25;

#define MAX_BOMB_COUNT 99
void SetBombsSeed(void); //sets random bombs position in the bombsPos array
bool CheckForBombPos(int); //checks if the index of the nodes array is occupied by a bomb

#define MAX_NODE_COUNT 625
SDL_Rect nodes[MAX_NODE_COUNT]; //every single node from top left
int bombsPos[MAX_BOMB_COUNT]; //nodes (from top-left) that are occupied by bombs

void CheckNode(int); //"opens" the clicked node
int CheckCloseBombsCount(int);//finding out which nodes should have what color
void ChangeNodeColor(int);

bool mouseClicked = false;

int GetMouseHoverNode(int, int); //returns a node that is under the mouse cursor
void ShowMap(void); //shows the layout of the map when the player losses

void SpawnBombs(void);
int openedNodes;

int main(int agrc, char* argv[]) {
	srand(time(0));

	if (!Initialize())
		return 1;

	bool quit = false;
	SDL_Event event;
	Uint32 lastTick = SDL_GetTicks();

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			mouseClicked = false;
			if (event.type == SDL_QUIT) {
				quit = true;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN)
				mouseClicked = true;
		}
		Uint32 currentTick = SDL_GetTicks();
		Uint32 diff = currentTick - lastTick;
		float elapsed = diff / 1000.0f;
		Update(elapsed, mouseClicked);
		lastTick = currentTick;
	}
	return 0;
}

bool Initialize(void) {
	window = SDL_CreateWindow("Minesweaper", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	if (!window) return false;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) return false;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SetBombsSeed();
	RenderBackNodes();

	SDL_RenderPresent(renderer);

	return true;
}

void Update(float elapsed, bool mouseClicked) {
	if (!mouseClicked) return;

	int x, y;
	SDL_GetMouseState(&x, &y);
	int nodeIndex = GetMouseHoverNode(x, y);
	CheckNode(nodeIndex);
	
	/*
	//printf("nodeIndex: %d\n", nodeIndex);
	SDL_SetRenderDrawColor(renderer, color_one.r, color_one.g, color_one.b, 255);
	SDL_Rect temp;
	temp.x = nodes[nodeIndex].x;
	temp.y = nodes[nodeIndex].y;
	temp.w = NODE_SIZE;
	temp.h = NODE_SIZE;
	SDL_RenderFillRect(renderer, &temp);
	*/
	SDL_RenderPresent(renderer);
}

void RenderBackNodes(void) {
	int currentNode = 0;
	int y = 0;
	for (int j = 0; j < columns; j++) {
		int x = 0;
		for (int i = 0; i < row; i++) {
			if ((i + j) % 2 == 0)
				SDL_SetRenderDrawColor(renderer, node_color_primary.r, node_color_primary.g, node_color_primary.b, 255);
			else
				SDL_SetRenderDrawColor(renderer, node_color_secondary.r, node_color_secondary.g, node_color_secondary.b, 255);
			
			SDL_Rect checkerRect{};
			checkerRect.x = x;
			checkerRect.y = y;
			checkerRect.w = NODE_SIZE;
			checkerRect.h = NODE_SIZE;
			SDL_RenderFillRect(renderer, &checkerRect);
			x += NODE_SIZE;
			nodes[currentNode++] = checkerRect;
		}
		y += NODE_SIZE;
	}
}

void SetBombsSeed(void) {	
	bool isNewPos = false;
	int randBombPos = NULL;
	int currentArrayPos = 0;

	for (int i = 0; i < MAX_BOMB_COUNT; i++) {	
		while (!isNewPos) {	
			randBombPos = rand() % MAX_NODE_COUNT;
			isNewPos = CheckForBombPos(randBombPos);
		}

		bombsPos[currentArrayPos++] = randBombPos;	
		isNewPos = false;
	}
}

bool CheckForBombPos(int selectedPos) {
	for (int i = 0; i < MAX_BOMB_COUNT; i++) {
		if (bombsPos[i] == selectedPos) {
			return false;
		}
	}
	return true;
}

void CheckNode(int nodePos) {	
	if (!CheckForBombPos(nodePos)) {
		ShowMap();
		Quit(1);
	}
		int numOfBombs = CheckCloseBombsCount(nodePos);
		ChangeNodeColor(numOfBombs);
		SDL_Rect node;
		node.x = nodes[nodePos].x;
		node.y = nodes[nodePos].y;
		node.w = NODE_SIZE;
		node.h = NODE_SIZE;
		SDL_RenderFillRect(renderer, &node);
		openedNodes++;
		if (openedNodes >= MAX_NODE_COUNT - MAX_BOMB_COUNT) Quit(0);
}

int CheckCloseBombsCount(int nodeIndex) {
	int bombCount = 0;
	if(!CheckForBombPos(nodeIndex - (row + 1)))
		bombCount++;
	if(!CheckForBombPos(nodeIndex - row))
		bombCount++;
	if(!CheckForBombPos(nodeIndex - (row - 1)))
		bombCount++;
	if(!CheckForBombPos(nodeIndex - 1))
		bombCount++;
	if (!CheckForBombPos(nodeIndex + 1))
		bombCount++;
	if (!CheckForBombPos(nodeIndex + (row - 1)))
		bombCount++;
	if (!CheckForBombPos(nodeIndex + row))
		bombCount++;
	if (!CheckForBombPos(nodeIndex + (row + 1)))
		bombCount++;

	return bombCount;
}

void ChangeNodeColor(int numOfBombs) {
	switch (numOfBombs) {
	case 0:
		SDL_SetRenderDrawColor(renderer, color_empty.r, color_empty.g, color_empty.b, 255);
		break;
	case 1:
		SDL_SetRenderDrawColor(renderer, color_one.r, color_one.g, color_one.b, 255);
		break;
	case 2:
		SDL_SetRenderDrawColor(renderer, color_two.r, color_two.g, color_two.b, 255);
		break;
	case 3:
		SDL_SetRenderDrawColor(renderer, color_three.r, color_three.g, color_three.b, 255);
		break;
	case 4:
		SDL_SetRenderDrawColor(renderer, color_four.r, color_four.g, color_four.b, 255);
		break;
	case 5:
		SDL_SetRenderDrawColor(renderer, color_five.r, color_five.g, color_five.b, 255);
		break;
	case 6:
		SDL_SetRenderDrawColor(renderer, color_six.r, color_six.g, color_six.b, 255);
		break;
	case 7:
		SDL_SetRenderDrawColor(renderer, color_seven.r, color_seven.g, color_seven.b, 255);
		break;
	case 8:
		SDL_SetRenderDrawColor(renderer, color_eight.r, color_eight.g, color_eight.b, 255);
		break;
	}
}

int GetMouseHoverNode(int mouseX, int mouseY) {
	int mouseNodeIndexX = mouseX - NODE_SIZE;
	int finalX = NULL;
	for (int i = 0; i < row; i++) {
		if (nodes[i].x >= mouseNodeIndexX) {
			finalX = nodes[i].x;
			break;
		}
	}
	int mouseNodeIndexY = mouseY - NODE_SIZE;
	int finalY = NULL;	
	for (int i = 0; i < MAX_NODE_COUNT; i += 25) {
		if (nodes[i].y >= mouseNodeIndexY) {
			finalY = i;
			break;
		}
	}
	for (int i = 0; i < MAX_NODE_COUNT; i++) {
		if (nodes[i].x == finalX && nodes[i].y == finalY) {
			return i;
		}
	}
}

void Quit(int status) {
	if (status == 1) printf("You lost!");
	else if (status == 0) printf("You won!");
	Sleep(2000);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void ShowMap(void) {
	//spawning bombs
	SpawnBombs();
	//spawning nodes	
	for (int i = 0; i < MAX_NODE_COUNT; i++) {
		if (!CheckForBombPos(i)) continue;
		int numOfBombs = CheckCloseBombsCount(i);
		ChangeNodeColor(numOfBombs);
		SDL_Rect node;
		node.x = nodes[i].x;
		node.y = nodes[i].y;
		node.w = NODE_SIZE;
		node.h = NODE_SIZE;
		SDL_RenderFillRect(renderer, &node);
	}
	SDL_RenderPresent(renderer);
}

void SpawnBombs(void) {
	SDL_SetRenderDrawColor(renderer, bomb_color.r, bomb_color.g, bomb_color.b, 255);
	for (int i = 0; i < MAX_BOMB_COUNT; i++) {
		SDL_Rect bomb;
		bomb.x = nodes[bombsPos[i]].x;
		bomb.y = nodes[bombsPos[i]].y;
		bomb.w = NODE_SIZE;
		bomb.h = NODE_SIZE;
		SDL_RenderFillRect(renderer, &bomb);
	}
	SDL_RenderPresent(renderer);
}