#pragma once

#include <SDL.h>
#include <SDL_opengl.h>

#include "real.h"
#include "botmath.h"
#include "bot.h"

class Graphics
{
	private:
	int width;
	int height;

	public:
	Graphics(int w, int h) : width(w), height(h)
	{
		SDL_Init(SDL_INIT_VIDEO);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		//SDL_Surface *drawContext = SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_HWSURFACE);
		/*SDL_Surface *drawContext = */SDL_SetVideoMode(width, height, 0, SDL_OPENGL);
		glShadeModel(GL_SMOOTH);
		glClearColor(0, 0, 0, 0);
		glEnable(GL_DEPTH_TEST);
		
		//glEnable(GL_POINT_SMOOTH);
		//glEnable(GL_LINE_SMOOTH);

		GLint iViewport[4];
		glGetIntegerv( GL_VIEWPORT, iViewport );
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		glOrtho(iViewport[0], iViewport[0]+iViewport[2], iViewport[1]+iViewport[3], iViewport[1], -1, 1);
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();
		glPushAttrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT );
		glDisable( GL_DEPTH_TEST );
		glDisable( GL_LIGHTING );
	}

	void drawCircle(float cx, float cy, float r) 
	{
		int num_segments = 5 * sqrtf(r);
        if(num_segments < 4) {
            num_segments = 4;
        }
		float theta = 2.0f * 3.1415926f / float(num_segments); 
		float c = cosf(theta);//precalculate the sine and cosine
		float s = sinf(theta);
		float t;

		float x = r;//we start at angle = 0 
		float y = 0.0f; 
		
		glBegin(GL_LINE_LOOP); 
		for(int ii = 0; ii < num_segments; ii++) 
		{ 
			glVertex2f(x + cx, y + cy);//output vertex 
			
			//apply the rotation matrix
			t = x;
			x = c * x - s * y;
			y = s * t + c * y;
		} 
		glEnd(); 
	}

	void paint(Bot bots[], int nbots)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glColor3f(1,0,0);
		for(int i = 0; i < nbots; ++i)
		{
			Bot &b = bots[i];
			drawCircle(b.pos.x, b.pos.y, b.radius);
		}

		SDL_GL_SwapBuffers();

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					exit(0);
					break;
			}
		}
	}
};

