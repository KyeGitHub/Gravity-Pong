#include "stdafx.h"
#include "MyGame.h"
#include <math.h>
#include <sstream>


CMyGame::CMyGame(void) : ball(0,600,12,CColor::Blue(),GetTime()),
	                     paddle(500,525, "paddle.bmp", GetTime()), 
						paddle2(500, 175, "paddle2.bmp", GetTime()),
						menu(500, 350, "menu.bmp", GetTime())
						 
						 //arena(500, 350,"arena.bmp",CColor::Black(), GetTime())
{
	// TODO: add initialisation here
	CSprite* pWall = new CSprite(CRectangle(150, 437.5, 255, 10), "wallhorz.bmp", CColor::Black(),GetTime() );
	pWall->Rotate(-45);
	wallList.push_back(pWall);
	pWall = new CSprite(CRectangle(150, 262.5, 255, 10), "wallhorz.bmp", CColor::Black(), GetTime());
	pWall->Rotate(45);
	wallList.push_back(pWall);
	pWall = new CSprite(CRectangle(600, 437.5, 255, 10), "wallhorz.bmp", CColor::Black(), GetTime());
	pWall->Rotate(45);
	wallList.push_back(pWall);
	pWall = new CSprite(CRectangle(600, 262.5, 255, 10), "wallhorz.bmp", CColor::Black(), GetTime());
	pWall->Rotate(-45);
	wallList.push_back(pWall);
	
	CSprite* pNet = new CSprite(CRectangle(paddle.GetX()-125, ((paddle.GetY()-(paddle.GetHeight()/2))-5), 245, 10), "wallhorz.bmp", CColor::Black(), GetTime());
	netList.push_back(pNet);
	pNet = new CSprite(CRectangle(paddle2.GetX()-125, ((paddle2.GetY() + (paddle2.GetHeight() / 2)) + 5), 245, 10), "wallhorz.bmp", CColor::Black(), GetTime());
	netList.push_back(pNet);
	/*wallList.push_back(new CSprite(CRectangle(0, 391, 600, 10), "wallhorz.bmp", GetTime()));
	wallList.push_back(new CSprite(CRectangle(0, 0, 10, 400), "wallvert.bmp", GetTime()));
	wallList.push_back(new CSprite(CRectangle(591, 0, 10, 400), "wallvert.bmp", GetTime()));*/
	
	// set score to zero
	score1 = 0;
	score2 = 0;
	countDown = 90;
	scored = false;
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (IsGameOverMode())
		return;
	if (!IsMenuMode())
	{
		// updating rock
		ball.Update(GetTime());

		// updating paddle
		paddle.Update(GetTime());
		paddle2.Update(GetTime());
		ballControl();
		paddleControl();
		for (CSprite* pWall : wallList)
		{
			pWall->Update(GetTime());
		}

		for (CSprite* pNet : netList)
		{
			pNet->Update(GetTime());
		}

		if (score1 > 5 || score2 > 5)
		{
			GameOver();
		}
	}
}

void CMyGame::OnDraw(CGraphics* g)
{
		// draw ball
	if (IsMenuMode())
	{
		menu.Draw(g);
		return;
	}
	else {

		if (!scored)
		{
			ball.Draw(g);
		}

		// draw paddle
		paddle.Draw(g);
		paddle2.Draw(g);
		for (CSprite* pWall : wallList)
		{
			pWall->Draw(g);
		}

		//arena.Draw(g);
		// print score
		*g << font(28) << color(CColor::Red()) << score1 << " : " << score2;

		if (IsGameOverMode())
			*g << font(46) << color(CColor::Green()) << vcenter << center << " GAME OVER ";
	}
}

void CMyGame::ballControl()
{
	const float r = 12;			// radius of the marble
	const float damp = 0.9f;	// damp factor for bouncing
	long dt = GetDeltaTime();	// time since the last frame (in milliseconds)
	ballTarget = CVector(500, 350);
	if (!held && !scored)	ball.Accelerate((ballTarget - ball.GetPosition())/50);
	if (scored)
	{
		ball.SetSpeed(0);
		ball.SetPosition(500, 350);
		countDown--;
		if (countDown < 1)
		{
			if (rand() % 2 == 0)
			{
				ball.SetPosition(rand() % GetWidth(), 0);
			}
			else
			{
				ball.SetPosition(rand() % GetWidth(), GetHeight());
			}
			countDown = 90;
			scored = false;
		}
	}
	if (paddle.HitTest(&ball))
	{
		held = true;
		storedVelocity = Reflect(ball.GetVelocity(), CVector(0, -1));
		storedSpeed = ball.GetSpeed();
		paddleHit = paddle.GetX() - ball.GetX();
		ball.SetVelocity(paddle.GetXVelocity(),0);
		ball.SetPosition(ball.GetX(), paddle.GetY() + r + (paddle.GetHeight() / 2));
		bounce.Play("bounce.wav");
	}
	if (paddle2.HitTest(&ball))
	{
		held = true;
		storedVelocity = Reflect(ball.GetVelocity(), CVector(0, -1));
		storedSpeed = ball.GetSpeed();
		paddleHit = paddle2.GetX() - ball.GetX();
		ball.SetVelocity(paddle2.GetXVelocity(), 0);
		ball.SetPosition(ball.GetX(), paddle2.GetY() - r - (paddle2.GetHeight() / 2));
		bounce.Play("bounce.wav");
	}

	for (CSprite *pNet : netList) 
	{
		if (pNet->HitTest(&ball))
		{
			if (ball.GetY() > 350)
			{
				scored = true;
				score2++;
				
			}
			else if (ball.GetY() < 350)
			{
				scored = true;
				score1++;
				
			}
		}
	}
	for (CSprite *pWall : wallList)
	{
		CVector v = ball.GetVelocity() * dt / 1000;
		CVector	t = pWall->GetPos() - ball.GetPos();
		float a = pWall->GetRotation();
		a = DEG2RAD(a);
		for (int i = 0; i < 4; i++)
		{
			CVector n = CVector(sin(a), cos(a));
			float R = 12;
			float X = pWall->GetWidth() / 2;
			float Y = pWall->GetHeight() / 2;

			if (i % 2 != 0)
			{
				swap(X, Y);
			}

			float vy = Dot(v, n);
			CVector d = t + (Y + R) * n;
			float dy = Dot(d, n);
			float f1 = dy / vy;

			float vx = Cross(v, n);
			float tx = Cross(t, n);
			float f2 = (tx - vx * f1) / (X + R);

			if (f1 >= 0 && f1 <= 1 && f2 >= -1 && f2 <= 1)
			{
				ball.SetVelocity(Reflect(ball.GetVelocity(), n)*damp);
				bounce.Play("bounce.wav");
			}
			a += M_PI / 2;
		}


	}
	if (ball.GetX() < 0)
	{
		ball.SetX(1000);
	}
	if(ball.GetX() > 1000)
	{
		ball.SetX(0);
	}
	if (ball.GetY() < 0)
	{
		ball.SetY(700);
	}
	if (ball.GetY() > 700)
	{
		ball.SetY(0);
	}

	if (held)
	{
		if (ball.GetY() > 350)
		{
			ball.SetX(paddle.GetX() - paddleHit);
		}
		else
		{
			ball.SetX(paddle2.GetX() - paddleHit);
		}
	}
}

void CMyGame::paddleControl()
{
	for (CSprite* pWall : wallList)
	{
		
		if (pWall->GetX() < 500 && (paddle.GetX() - paddle.GetWidth()/2 < 370) && (pWall->GetY() > 350))
		{
			paddle.SetXVelocity((paddle.GetXVelocity()*-1)* 0.6f);
			paddle.SetX(371+(paddle.GetWidth()/2));
			if (held)
			{
				ball.SetVelocity(paddle.GetVelocity());
			}
			
		}
		else if (pWall->GetX() > 500 && (paddle.GetX() + paddle.GetWidth() / 2 > 635) && (pWall->GetY() > 350))
		{
			paddle.SetXVelocity((paddle.GetXVelocity()*-1)* 0.6f);
			paddle.SetX(634 - (paddle.GetWidth() / 2));
			if (held)
			{
				ball.SetVelocity(paddle.GetVelocity());
			}
		}

		if (pWall->GetX() < 500 && (paddle2.GetX() - paddle2.GetWidth() / 2 < 370) && (pWall->GetY() < 350))
		{
			paddle2.SetXVelocity((paddle2.GetXVelocity()*-1)* 0.6f);
			paddle2.SetX(371 + (paddle2.GetWidth() / 2));
			if (held)
			{
				ball.SetVelocity(paddle2.GetVelocity());
			}
		}
		else if (pWall->GetX() > 500 && (paddle2.GetX() + paddle2.GetWidth() / 2 > 635) && (pWall->GetY() > 350))
		{
			paddle2.SetXVelocity((paddle2.GetXVelocity()*-1)* 0.6f);
			paddle2.SetX(634 - (paddle2.GetWidth() / 2));
			if (held)
			{
				ball.SetVelocity(paddle2.GetVelocity());
			}
		}
	}
	
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	
	score1 = 0;
	score2 = 0;
}

// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
	

	// set score to 0
	
	
	
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
	music.Play("music.wav");
}

// called when Game is Over
void CMyGame::OnGameOver()
{
	music.Stop();
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_F2)
		NewGame();
	if (sym == SDLK_RETURN && IsMenuMode())
	{
		StartGame();
	}

	if ((sym == SDLK_a))
	{
		paddle2.SetXVelocity(paddle2.GetXVelocity()-50);
		
	}

	if ((sym == SDLK_d))
	{
		paddle2.SetXVelocity(paddle2.GetXVelocity()+50);
		
	}
	if ((sym == SDLK_s))
	{
		paddle2.SetXVelocity((paddle2.GetXVelocity() / 10));
		
	}
	if ((sym == SDLK_w && held && ball.GetY() < 350))
	{
		ball.SetVelocity(storedVelocity);
		ball.SetSpeed(500);
		held = false;
		bounce.Play("release.wav");
	}

	if ((sym == SDLK_LEFT))
	{
		paddle.SetXVelocity(paddle.GetXVelocity() -50);
		
	}

	if ((sym == SDLK_RIGHT))
	{
		paddle.SetXVelocity(paddle.GetXVelocity()+50);
		
	}
	if ((sym == SDLK_DOWN))
	{
		paddle.SetXVelocity(paddle.GetXVelocity()/10);
		
	}
	if ((sym == SDLK_UP && held && ball.GetY() > 350))
	{
		ball.SetVelocity(storedVelocity);
		ball.SetSpeed(500);
		bounce.Play("release.wav");
		held = false;
	}
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{ 
	
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	ballTarget = CVector(x, y);
	ball.Accelerate(ballTarget - ball.GetPosition());
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
