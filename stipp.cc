#include <iostream>
#include <math.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <GL/glew.h>
#include <string>
#include <primitive3d.hh>
#include "menu.hh"

S3DImage *texture0;
S3DImage *texture1;
int width = 700;
int height = 700;
float angle = 0;



// --------------------------------------------------
void drawGear(bool ccw)
{
   glBindTexture(GL_TEXTURE_2D, texture1->getTexture2D());
   glPushMatrix();
   {
      glColor4f(.58, .36, .1, .5);
      if (!ccw)
	 glRotatef(angle, 0, 0, 1);
      else
	 glRotatef(-angle, 0, 0, 1);
      glTranslatef(-.25, -.25, 0);
      S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, .5, .5, true);
   }
   glPopMatrix();
}


// --------------------------------------------------
void drawBG(void)
{
   float w = 1;
   float h = 1;
   float r = 1;


   glViewport(0, 0, width, height);

   glClearColor(0.9, 0.9, 1.0, 0.0);
   glClearColor(.58, .36, .1, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


   if (height < width)
      r = 0.6 * width / (float) height;
   else
      r = 0.6 * height / (float) width;

   glDisable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);
   glDepthMask(false); // Disable the z buffer 

   // Draw something:
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   {
      glLoadIdentity();
//      gluPerspective(45, 1.33, 1.0, 60.0);
//      h = height / width;
      glOrtho(0, r, 0, r, -10, 10);

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      {
	 glLoadIdentity();

	 glEnable(GL_BLEND);
	 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	 glEnable(GL_TEXTURE_2D);

	 glBindTexture(GL_TEXTURE_2D, texture0->getTexture2D());
	 S3DPrimitive3D::drawPlane(GL_POLYGON, 0, 0, 0, 1, 1, true);

	 // select modulate to mix texture with color for shading
	 glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	 glPushMatrix();
	 {
	    glTranslatef(0.5, 0.5, 0);
	    drawGear(true);
	 }
	 glPopMatrix();
	 
	 glPushMatrix();
	 {
	    glTranslatef(1 - 0.05, 0.5 - 0.16, 0);
	    drawGear(false);
	 }
	 glPopMatrix();
	 
	 glPushMatrix();
	 {
	    glTranslatef(+ 0.05, 0.5 - 0.16, 0);
	    drawGear(false);
	 }
	 glPopMatrix();
      }
      glPopMatrix();

      glMatrixMode(GL_PROJECTION);
   }
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);

   // select modulate to mix texture with color for shading
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   angle += 0.01;
}


// --------------------------------------------------
int main (int argc, char *argv[])
{
   const GLubyte* strm;
   const SDL_VideoInfo* info = NULL;
   SDL_Surface *screen;
   SDL_Event event;
   TTF_Font *fnt;
   GLenum err;
   int bpp = 0;
   int flags = 0;
   int quit = 0;
   char *path, *filename;
   int i;
   float version;
   SMenu *menu;
   SDL_Rect **lmodes;



   path = new char[strlen(argv[0]) + 4];
   path[0] = '\0';
   strcpy(path, *argv);

   i = strlen(*argv) - 1;
   while (i >= 0)
   {
#ifdef WINDOWS    
      if ( (path[i] == '/') || (path[i] == '\\') )
#else
      if (path[i] == '/')
#endif
      {
         path[i] = '\0';
         i = 0;
      }

      i --;
   }
   std::cerr << "path = " << path << std::endl;



   /* ----- SDL init --------------- */
   if(SDL_Init(SDL_INIT_VIDEO) < 0) 
   {
      std::cerr << "Video initialization failed: " << SDL_GetError() << "\n";
      exit(-1);
   }
   
   atexit(SDL_Quit);
	
   info = SDL_GetVideoInfo();
   bpp = info->vfmt->BitsPerPixel;
   std::cerr << bpp << std::endl;
   
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


   /*	flags = SDL_OPENGL | SDL_FULLSCREEN; */
   flags = SDL_OPENGL; //  | SDL_RESIZABLE;


   /* ----- Setting up the screen surface --------------- */

   lmodes = SDL_ListModes(0, flags);
   if (lmodes == 0)
   {
      std::cerr << "No resolutions available." << std::endl;
      exit (-1);
   }
   else
      if (lmodes == (SDL_Rect**)-1) 
      {
	 std::cerr << "All resolutions available." << std::endl;
	 width = 1024;
	 height = 700;
      }
      else
      {
	 for (int i=0; lmodes[i]; ++i)
	    std::cerr << lmodes[i]->w << " x " << lmodes[i]->h << std::endl;

      }


   screen = SDL_SetVideoMode(width, height, bpp, flags);
   if(screen == 0) 
   {
      std::cerr << "Video mode set failed: " << SDL_GetError() << "\n";
      exit(-1);
   }

   SDL_WM_SetCaption("Stippling PRO", 0);


   /* ----- Checking for OpenGL 2 --------------- */
   strm = glGetString(GL_VENDOR);
   std::cerr << "Vendor: " << strm << "\n";
   strm = glGetString(GL_RENDERER);
   std::cerr << "Renderer: " << strm << "\n";
   strm = glGetString(GL_VERSION);
   std::cerr << "OpenGL Version: " << strm << "\n";

   version = atof((const char *) strm);

   if (version < 2) 
   {
      std::cerr << "Warning: OpenGL 2 not supported!\n";
   }
   strm = glGetString(GL_VERSION);
   std::cerr << "Detected OpenGL Version >= " << version << "\n";


   strm = glGetString(GL_SHADING_LANGUAGE_VERSION);
   std::cerr << "GLSL Version: " << strm << "\n";


   // Glew init:
   err = glewInit();
   if (GLEW_OK != err)
   {
      /* Problem: glewInit failed, something is seriously wrong. */
      std::cerr << "Error: " << glewGetErrorString(err) << "\n";
      exit (-1);
   }

   std::cerr << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << "\n";


   // UI:
   // ---------------------------------------------------------- 
   SDL_ShowCursor(SDL_DISABLE);
   if(TTF_Init() == -1)
      return 1;


   // Make the menu
   menu = new SMenu(path, width, height);

   // --- Load preimages:
   filename = new char[std::strlen(path) + 1000];
   texture0 = new S3DImage();
   filename[0] = '\0';
   std::sprintf (filename,"%s/../media/papers/old_paper.jpg", path);
   texture0->load(filename);
   texture0->setTexture2D(GL_TEXTURE0, GL_NEAREST, GL_CLAMP, GL_REPLACE);

   texture1 = new S3DImage();
   filename[0] = '\0';
   std::sprintf (filename,"%s/../icons/gear.png", path);
   texture1->load(filename);
   texture1->setTexture2D(GL_TEXTURE0, GL_NEAREST, GL_CLAMP, GL_REPLACE);

   delete [] filename;

   /* ----- Event cycle --------------- */
   quit = 0;
   while (!quit) 
   {
      glPushAttrib(GL_VIEWPORT_BIT);
      {
	 drawBG();
      }
      glPopAttrib();

      while (SDL_PollEvent(&event)) 
      {
	 switch (event.type) 
	 {
	    // If you click on the close button:
	    case SDL_QUIT: {
	       quit = 1;
	    } break;

	    case SDL_KEYDOWN: {
	       if (event.key.keysym.sym == SDLK_ESCAPE)
		  quit = 1;
	       else
		  if (event.key.keysym.sym == 'f')
		  {
		     if (flags & SDL_FULLSCREEN)
			flags = SDL_OPENGL  | SDL_RESIZABLE;
		     else
			flags = SDL_OPENGL  | SDL_FULLSCREEN;

		     screen = SDL_SetVideoMode(width, height, bpp, flags);
		     if(screen == 0) 
		     {
			std::cerr << "Video mode set failed: " 
				  << SDL_GetError() << "\n";
			exit(-1);
		     }
		  }

	    } break;

	    case SDL_VIDEORESIZE: {
	       screen = SDL_SetVideoMode(event.resize.w, 
	                                 event.resize.h, 
                                         bpp, flags); 
	       if(screen == 0)
	       {
		  std::cerr << "Video resize failed: " << SDL_GetError() 
			    << "\n";
		  exit (-1);
	       }
	       else
	       {
		  // Resize = event.resize.w;
		  height = event.resize.h;

		  // When resize, the viewport must be updated:
		  menu->setSize(width, height);
	       }


            } break;

	    default : {
	    } break;	 
	 }

         menu->readEventsSDL(&event);
      }


      menu->draw();
//      SDL_GL_SwapBuffers();
      SDL_Delay(1);
   }
  
   SDL_Quit();

   delete menu;
   delete texture0;
   delete [] path;

  
   return 0;
}


