#pragma once

class CMyGame : public CGame
{
	CSprite paddle;  // paddle sprite declaration
	CSprite paddle2;
	CSprite arena;
	CSprite menu;
	CSpriteOval ball;
	CSpriteList wallList;
	CSpriteList netList;
	CVector ballTarget;
	CVector storedVelocity;
	float storedSpeed;
	int score1;	
	int score2;
	int countDown;
	bool scored;
	double paddleHit;
	bool held;
	void ballControl();
	void paddleControl();

	CSoundPlayer bounce;
	CSoundPlayer music;

public:
	CMyGame(void);	 // Constructor
	~CMyGame(void);  // Destructor

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);
};
