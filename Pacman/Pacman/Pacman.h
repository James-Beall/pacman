#pragma once

// If Windows and not in Debug, this will run without a console window
// You can use this to output information when debugging using cout or cerr
#ifdef WIN32 
	#ifndef _DEBUG
		#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#endif
#endif

//Area for defining data
#define MUNCHIECOUNT 3
#define GHOSTCOUNT 1

// Just need to include main header file
#include "S2D/S2D.h"

// Reduces the amount of typing by including all classes in S2D namespace
using namespace S2D;

// Structure Definition  Pscman
struct Player { 
	Vector2* Position;
	Rect* SourceRect;
	Texture2D* Texture;
	int Direction;
	int Frame;
	int currentFrameTime;
	float speedMultiplier; 
	bool dead;
};

// Structure Definition of moving Enemys
struct MovingEnemy
{
	Vector2* position;
	Texture2D* texture;
	Rect* sourceRect;
	int direction;
	float speed;
};

// Structure Definition Enemy/Munchie
struct Enemy {
	int frameCount;
	Rect* rect;
	Vector2* position;
	Rect* sourceRect;
	Texture2D* texture;
	int currentFrameTime;
	int frameTime;
};

// Structure Definition Menu 
struct Menu {
	Texture2D* background;
	Rect* rectangle;
	Vector2* stringPosition;
	bool _keyDown;
	bool _checkOpen;
};

// Declares the Pacman class which inherits from the Game class.
// This allows us to overload the Game class methods to help us
// load content, draw and update our game.
class Pacman : public Game
{
private:

	void Input(int elapsedTime, Input::KeyboardState* state, Input::MouseState* mouseState);
	void CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey);
	void CheckStart(Input::KeyboardState* state, Input::Keys startKey);
	void CheckViewportCollision();
	void UpdatePacman(int elapsedTime);
	void UpdateMunchie(Enemy* _munchies,int elapsedTime);
	void CheckMunchieCollisions(Enemy* _munchies);
	void CheckGhostCollisions();
	void UpdateGhost(MovingEnemy*, int elapsedTime);

	// Data to represent Pacman
	//Vector2* _pacmanPosition;
	//Rect* _pacmanSourceRect;
	//Texture2D* _pacmanTexture;
	//int _pacmanDirection;
	//int _pacmanFrame;
	//int _pacmanCurrentFrameTime;
	Player* _pacman;

	// Data to represent Munchie
	//int _munchieFrameCount;
	//Rect* _munchieRect;
	//Rect* _munchieSourceRect;
	//Texture2D* _munchieTexture;
	//int _munchieCurrentFrameTime;
	Enemy* _munchies[MUNCHIECOUNT];
	Enemy* _cherry;

	MovingEnemy* _ghosts[GHOSTCOUNT];
	SoundEffect* _pop;

	// Position for String
	Vector2* _stringPosition;

	//Constant data for Game Variables 
	const float _cPacmanSpeed;
	const int _cPacmanFrameTime;
	const int _cMunchieFrameTime;

	// Data for Menu
	Menu* _startMenu;
	Menu* _pauseMenu;

public:
	/// <summary> Constructs the Pacman class. </summary>
	Pacman(int argc, char* argv[]);

	/// <summary> Destroys any data associated with Pacman class. </summary>
	virtual ~Pacman();

	/// <summary> All content should be loaded in this method. </summary>
	void virtual LoadContent();

	/// <summary> Called every frame - update game logic here. </summary>
	void virtual Update(int elapsedTime);

	/// <summary> Called every frame - draw game here. </summary>
	void virtual Draw(int elapsedTime);
};