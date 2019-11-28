#include "Pacman.h"
#include <iostream>
#include <sstream>
#include <random>

//Constructor Method 
Pacman::Pacman(int argc, char* argv[]) : Game(argc, argv), _cPacmanSpeed(0.1f), _cPacmanFrameTime(250), _cMunchieFrameTime(500)
{
	//local variable
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i] = new Enemy();
		_munchies[i]->currentFrameTime = 0;
		_munchies[i]->frameCount = 0;
		_munchies[i]->frameTime = rand() % 500 + 50;
	}

	// Initialise member variables 
	_pacman = new Player(); 
	_cherry = new Enemy();
	/*_ghosts[0] = new MovingEnemy();*/
	/*_munchie = new Enemy();*/
	_pop = new SoundEffect();

	//initialise ghost character
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i] = new MovingEnemy();
		_ghosts[i]->direction = 0;
		_ghosts[i]->speed = 0.2f;
	}

	//Menus
	_startMenu = new Menu();
	_startMenu->_checkOpen = false;
	_startMenu->_keyDown = false;
	_pauseMenu = new Menu();
	_pauseMenu->_checkOpen = false;
	_pauseMenu->_keyDown = false;

	// Remaining variable initialisation
	_pacman->Direction = 0;
	_pacman->dead = false;
	_pacman->currentFrameTime = 0;
	_pacman->Frame = 0;
	//_munchie->frameCount = 0;
	//_munchie->currentFrameTime = 0;
	_cherry->frameCount = 0;
	_cherry->currentFrameTime = 0;
	_pacman->speedMultiplier = 1.0f;

//Initialise important Game aspects
Audio::Initialise();
Graphics::Initialise(argc, argv, this, 1024, 768, false, 25, 25, "Pacman", 60);
Input::Initialise();

if (!Audio::IsInitialised())
{
	std::cout << "Audio is nor initialised" << std::endl;
}
else
{
	std::cout << "Audio is initialised" << std::endl;
}
if (!_pop->IsLoaded())
{
	std::cout << "_pop member sound effect has not loaded" << std::endl;
}


// Start the Game Loop - This calls Update and Draw in game loop
Graphics::StartGameLoop();
}

// Destructor Method
Pacman::~Pacman()
{
	//Clean up munchie texutre
	delete _munchies[0]->texture;

	for (int nCount = 0; nCount < MUNCHIECOUNT; ++nCount)
	{
		delete _munchies[nCount]->rect;
		delete _munchies[nCount]->sourceRect;
		delete _munchies[nCount];
	}

	//Order of operations is important, array deleted last
	delete[] _munchies;

	// Clean up pointers within the Pacman structure
	delete _pacman->Texture;
	delete _pacman->SourceRect;
	delete _pacman->Position;

	//comment later
	delete _ghosts[0]->texture;

	for (int nCount = 0; nCount < GHOSTCOUNT; ++nCount)
	{
		delete _ghosts[nCount]->position;
		delete _ghosts[nCount]->sourceRect;
		delete _ghosts[nCount];
	}

	//comment lateer
	delete[] _ghosts;

	//comment later 
	delete _pop;

	// Clean up the Pacman structure pointer 
	delete _pacman;
}

void Pacman::LoadContent()
{
	_pop->Load("Sounds/pop.wav");

	// Load Pacman
	_pacman->Texture = new Texture2D();
	_pacman->Texture->Load("Textures/Pacman.tga", false);
	_pacman->Position = new Vector2(350.0f, 350.0f);
	_pacman->SourceRect = new Rect(0.0f, 0.0f, 32, 32);

	//Initialise ghost character
	Texture2D* ghostTrex = new Texture2D();
	ghostTrex->Load("Textures/GhostBlue.png", false);
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		_ghosts[i]->texture= ghostTrex;
		_ghosts[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_ghosts[i]->sourceRect = new Rect(0.0f, 0.0f, 20, 20);
	}

	// Load Munchie
	Texture2D* munchieTrex = new Texture2D();
	munchieTrex->Load("Textures/Munchie.png", false);
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		_munchies[i]->texture = munchieTrex;
		//_munchies[i]->texture = new Texture2D();
		//_munchies[i]->texture->Load("Textures/Munchie.png", true);
		_munchies[i]->position = new Vector2((rand() % Graphics::GetViewportWidth()), (rand() % Graphics::GetViewportHeight()));
		_munchies[i]->sourceRect = new Rect(0.0f, 0.0f, 12, 12);
	}

	//Load Cherry 
	_cherry->texture = new Texture2D();
	_cherry->texture->Load("Textures/spriteSheet.png", true);
	_cherry->position = new Vector2(100.0f, 450.0f);
	_cherry->sourceRect = new Rect(0.0f, 0.0f, 32, 32);

	// Set string position
	_stringPosition = new Vector2(10.0f, 25.0f);

	// Set Menu Paramters
	_pauseMenu->background = new Texture2D();
	_pauseMenu->background->Load("Textures/Transparency.png", false);
	_pauseMenu->rectangle = new Rect(0.0f, 0.0f, Graphics::GetViewportWidth(), Graphics::GetViewportHeight());
	_pauseMenu->stringPosition = new Vector2(Graphics::GetViewportWidth() / 2.0f, Graphics::GetViewportHeight() / 2.0f);

}

void Pacman::Update(int elapsedTime)
{
	// Gets the current state of the keyboard
	Input::KeyboardState* keyboardState = Input::Keyboard::GetState();
	Input::MouseState* mouseState = Input::Mouse::GetState();
	//checks if space has been pressed

	CheckStart(keyboardState, Input::Keys::SPACE);

	if (_startMenu->_checkOpen)
	{
		CheckPaused(keyboardState, Input::Keys::P);
		if (!_pauseMenu->_checkOpen)
		{
			Input(elapsedTime, keyboardState, mouseState);
			UpdatePacman(elapsedTime);
			for (int i = 0; i < MUNCHIECOUNT; i++)
			{
				UpdateMunchie(_munchies[i], elapsedTime);
				CheckMunchieCollisions(_munchies[i]);
			}
			CheckViewportCollision();
			UpdateMunchie(_cherry, elapsedTime);
			for (int i = 0; i < GHOSTCOUNT; i++)
			{
				UpdateGhost(_ghosts[i], elapsedTime);
			}
			CheckGhostCollisions();
		}
	}
}

void Pacman::Input(int elapsedTime, Input::KeyboardState * state, Input::MouseState * mouseState)
{

	// Handles Mouse Input - Reposition Cherry 
	if (mouseState->LeftButton == Input::ButtonState::PRESSED)
	{
		_cherry->position->X = mouseState->X;
		_cherry->position->Y = mouseState->Y;
	}

	// Checks if R key is pressed
	if (state->IsKeyDown(Input::Keys::R))
	{
		// Randomizes cherry position
		_cherry->position->X = Graphics::GetViewportWidth() % rand();
		_cherry->position->Y = Graphics::GetViewportHeight() % rand();
	}

	// Speed Multipler 
	if (state->IsKeyDown(Input::Keys::LEFTSHIFT))
	{
		// Apply multipler
		_pacman->speedMultiplier = 2.0f;
	}
	else
	{
		// Reset Multipler
		_pacman->speedMultiplier = 1.0f;
	}

	float pacmanSpeed = _cPacmanSpeed * elapsedTime * _pacman->speedMultiplier;

	// Checks if movement keys are pressed
	if (state->IsKeyDown(Input::Keys::D))
	{
		_pacman->Position->X += pacmanSpeed; //Moves Pacman across X axis
		_pacman->Direction = 0;
	}
	else if (state->IsKeyDown(Input::Keys::A))
	{
		_pacman->Position->X -= pacmanSpeed; //Moves Pacman across X axis
		_pacman->Direction = 2;
	}
	else if (state->IsKeyDown(Input::Keys::W))
	{
		_pacman->Position->Y -= pacmanSpeed; //Moves Pacman across Y axis
		_pacman->Direction = 3;
	}
	else if (state->IsKeyDown(Input::Keys::S))
	{
		_pacman->Position->Y += pacmanSpeed; //Moves Pacman across Y axis
		_pacman->Direction = 1;
	}
}

void Pacman::CheckStart(Input::KeyboardState* state, Input::Keys startKey)
{
	if (state->IsKeyDown(Input::Keys::SPACE))
	{
		_startMenu->_checkOpen = true;
	}
}
void Pacman::CheckPaused(Input::KeyboardState* state, Input::Keys pauseKey)
{
	//Checks if game is paused 
	if (state->IsKeyDown(Input::Keys::P) && !_pauseMenu->_keyDown)
	{
		_pauseMenu->_checkOpen = !_pauseMenu->_checkOpen;
		_pauseMenu->_keyDown = true;
	}

	if (state->IsKeyUp(Input::Keys::P))
	{
		_pauseMenu->_keyDown = false;
	}
		
}
void Pacman::CheckViewportCollision()
{
	//Checks if pacman is tryingg to disappear
	if (_pacman->Position->X + _pacman->SourceRect->Width > Graphics::GetViewportWidth())//checks if pacman hits right wall
	{
		_pacman->Position->X = 0;
	}
	if (_pacman->Position->X + _pacman->SourceRect->Width < _pacman->SourceRect->Width)//checks if pacman hits left wall 
	{
		_pacman->Position->X = Graphics::GetViewportWidth() - _pacman->SourceRect->Width;
	}
	if (_pacman->Position->Y + _pacman->SourceRect->Height > Graphics::GetViewportHeight())//checks if pacman hits bottom wall 
	{
		_pacman->Position->Y = 0;
	}
	if (_pacman->Position->Y + _pacman->SourceRect->Height < _pacman->SourceRect->Height)//checks if pacman hits top wall 
	{
		_pacman->Position->Y = Graphics::GetViewportHeight() - _pacman->SourceRect->Height;
	}
}
void Pacman::UpdatePacman(int elapsedTime)
{
	//checks time since update function last ran
	_pacman->currentFrameTime += elapsedTime;
	if (_pacman->currentFrameTime > _cPacmanFrameTime)
	{
		_pacman->Frame++;
		if (_pacman->Frame >= 2)
			_pacman->Frame = 0;
		_pacman->currentFrameTime = 0;
	}
	_pacman->SourceRect->Y = _pacman->SourceRect->Height * _pacman->Direction; // change pacman sprite direction based upon input 
	_pacman->SourceRect->X = _pacman->SourceRect->Width * _pacman->Frame; // pacman animation
}
void Pacman::UpdateMunchie(Enemy* _munchies,int elapsedTime)
{
		// munchie now time dependant
		_munchies->currentFrameTime += elapsedTime;
		if (_munchies->currentFrameTime > _cMunchieFrameTime)
		{
			_munchies->frameCount++;
			if (_munchies->frameCount >= 2)
				_munchies->frameCount = 0;
			_munchies->currentFrameTime = 0;
		}
		_munchies->sourceRect->X = _munchies->sourceRect->Width * _munchies->frameCount; // munchie animation 

}

void Pacman::UpdateGhost(MovingEnemy* ghost, int elapsedTime)
{
	if (ghost->direction == 0) //Moves Right 
	{
		ghost->position->X += ghost->speed * elapsedTime;
	}
	else if (ghost->direction == 1) //Moves Left 
	{
		ghost->position->X -= ghost->speed * elapsedTime;
	}

	if (ghost->position->X + ghost->sourceRect->Width >=
		Graphics::GetViewportWidth()) //Hits Right edge 
	{
		ghost->direction = 1; //Change direction 
	}
	else if (ghost->position->X <= 0) //Hits left edge 
	{
		ghost->direction = 0; //Change direction 
	}
}

void Pacman::CheckMunchieCollisions(Enemy* _munchies)
{
	//local variables 
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;

	bottom2 = _munchies->position->Y + _munchies->sourceRect->Height;
	left2 = _munchies->position->X;
	right2 = _munchies->position->X + _munchies->sourceRect->Width;
	top2 = _munchies->position->Y;
	//if ((_munchies->position->X == _pacman->Position->X) && (_munchies->position->Y == _pacman->Position->Y))
	if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
	{
		Audio::Play(_pop);
	}
}

void Pacman::CheckGhostCollisions()
{
	// Local Variables
	int i = 0;
	int bottom1 = _pacman->Position->Y + _pacman->SourceRect->Height;
	int bottom2 = 0;
	int left1 = _pacman->Position->X;
	int left2 = 0;
	int right1 = _pacman->Position->X + _pacman->SourceRect->Width;
	int right2 = 0;
	int top1 = _pacman->Position->Y;
	int top2 = 0;

	for (i = 0; i < GHOSTCOUNT; i++)
	{
		// Populate variables with Ghost data
		bottom2 =_ghosts[i]->position->Y + _ghosts[i]->sourceRect->Height;
		left2 = _ghosts[i]->position->X;
		right2 =_ghosts[i]->position->X + _ghosts[i]->sourceRect->Width;
		top2 = _ghosts[i]->position->Y;

		if ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2))
		{
			_pacman->dead = true;
			i = GHOSTCOUNT;
		}
	}
}

void Pacman::Draw(int elapsedTime)
{
	// Allows us to easily create a string
	std::stringstream stream;
	stream << "Pacman X: " << _pacman->Position->X << " Y: " << _pacman->Position->Y;

	SpriteBatch::BeginDraw(); // Starts Drawing

	if (!_pacman->dead)
	{
		SpriteBatch::Draw(_pacman->Texture, _pacman->Position, _pacman->SourceRect); // Draws Pacman
	}

	//draw ghosts
	for (int i = 0; i < GHOSTCOUNT; i++)
	{
		SpriteBatch::Draw(_ghosts[i]->texture, _ghosts[i]->position, _ghosts[i]->sourceRect);
	}
	for (int i = 0; i < MUNCHIECOUNT; i++)
	{
		SpriteBatch::Draw(_munchies[i]->texture, _munchies[i]->position, _munchies[i]->sourceRect, Vector2::Zero, 1.0f, 0.0f, Color::White, SpriteEffect::NONE); // draws munchie
	}

	SpriteBatch::Draw(_cherry->texture, _cherry->position, _cherry->sourceRect); // Draws Cherry

	//pause text if game is paused
	if (_pauseMenu->_checkOpen)
	{
		std::stringstream menuStream;
		menuStream << "PAUSED!";
		SpriteBatch::Draw(_pauseMenu->background, _pauseMenu->rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _pauseMenu->stringPosition, Color::Red);
	}

	//start text will appear untill start is true 
	if (!_startMenu->_checkOpen)
	{
		std::stringstream menuStream;
		menuStream << "Press space to begin!";
		SpriteBatch::Draw(_pauseMenu->background, _pauseMenu->rectangle, nullptr);
		SpriteBatch::DrawString(menuStream.str().c_str(), _pauseMenu->stringPosition, Color::Red);
	}

	// Draws String
	SpriteBatch::DrawString(stream.str().c_str(), _stringPosition, Color::Green);
	SpriteBatch::EndDraw(); // Ends Drawing
}
