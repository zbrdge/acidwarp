/* ACID WARP (c)Copyright 1992, 1993 by Noah Spurrier
 * All Rights reserved. Private Proprietary Source Code by Noah Spurrier
 * Ported to Linux by Steven Wills
 * Ported to SDL by Boris Gjenero
 */

#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>

#include "handy.h"
#include "acidwarp.h"
#include "display.h"

static SDL_Surface *surface = NULL, *screen = NULL;
static int disp_DrawingOnSurface;
#ifdef HAVE_PALETTE
static int disp_UsePalette;
#endif
static int fullscreen = 0;
static int scaling = 1;
// Save window size when in full screen
static int winwidth = 0;
static int winheight;

static void disp_SDLFatal(const char *msg) {
  fprintf(stderr, "SDL error while %s: %s", msg, SDL_GetError());
  exit(-1);
}

void disp_setPalette(unsigned char *palette)
{
  static SDL_Color sdlPalette[256];
  int i;
  for(i=0;i<256;i++) {
    sdlPalette[i].r = palette[i*3+0] << 2;
    sdlPalette[i].g = palette[i*3+1] << 2;
    sdlPalette[i].b = palette[i*3+2] << 2;
  }

#ifdef HAVE_PALETTE
  if (disp_UsePalette) {
    /* Simply change the palette */
    SDL_SetPalette(screen, SDL_PHYSPAL, sdlPalette, 0, 256);
  } else
#endif
  {
    /* Update colours in software surface, blit it to the screen
     * with updated colours, and then show it on the screen.
     */
    SDL_SetColors(surface, sdlPalette, 0, 256);
    if (surface != screen) {
      SDL_BlitSurface(surface, NULL, screen, NULL);
    }
    SDL_Flip(screen);
  }
}

void disp_beginUpdate(void)
{
  /* Locking only needed at this point if drawing routines directly draw
   * on a surface, and that surface needs locking.
   */
  if (disp_DrawingOnSurface && SDL_MUSTLOCK(surface)) {
    if (SDL_LockSurface(surface) != 0) {
      disp_SDLFatal("locking surface when starting update");
    }
    buf_graf = surface->pixels;
    buf_graf_stride = surface->pitch;
  }
}

void disp_finishUpdate(void)
{
  if (!disp_DrawingOnSurface) {
    int row;
    unsigned char *outp, *inp = buf_graf;

    /* This means drawing was on a separate buffer and it needs to be
     * copied to the surface. It also means the surface hasn't been locked.
     */
    if (SDL_MUSTLOCK(surface)) {
      if (SDL_LockSurface(surface) != 0) {
        disp_SDLFatal("locking surface when ending update");
        exit(-1);
      }
    }
    outp = surface->pixels;

  if (scaling == 1) {	  
    for (row = 0; row <= YMax; row++) {	 
      memcpy(outp, inp, XMax+1);
	  outp += surface->pitch;
	  inp += XMax + 1;
    }
  } else if (scaling == 2) {
    unsigned char *outp2 = outp + surface->pitch;
	int skip = (surface->pitch - XMax - 1) * 2;
	int col;
	unsigned char c;
    for (row = 0; row <= YMax; row++) {	 
	  for (col = 0; col <= XMax; col++) { 
	    c = *(inp++);
		*(outp++) = c;
		*(outp++) = c;
		*(outp2++) = c;
		*(outp2++) = c;
	  }
	  outp += skip;
	  outp2 += skip;
    }
  }
  }

  SDL_UnlockSurface(surface);
  if (surface != screen) {
    SDL_BlitSurface(surface, NULL, screen, NULL);
  }
  SDL_Flip(screen);
}

void disp_processInput(void) {
  SDL_Event event;
 
  char keyHit = 0;
  
  while ( SDL_PollEvent(&event) > 0 ) {
    switch (event.type) {
      case SDL_VIDEOEXPOSE:
        /* Redraw parts that were overwritten. (This is unlikely with
         * modern compositing window managers */
        if (surface != screen) {
          SDL_BlitSurface(surface, NULL, screen, NULL);
          SDL_Flip(screen);
        } else {
          /* Copy from buf_graf to screen */
          disp_beginUpdate();
          disp_finishUpdate();
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
		fullscreen = !fullscreen;
        disp_init();
		break;
      case SDL_KEYDOWN:
        ///* Ignore key releases */
        //if ( event.key.state == SDL_RELEASED ) {
        //  break;
        //}
        /* Ignore ALT-TAB for windows */
        if ( (event.key.keysym.sym == SDLK_LALT) ||
             (event.key.keysym.sym == SDLK_TAB) ) {
          break;
        } else if (event.key.keysym.sym == SDLK_UP) {
          handleinput(6);
		} else if (event.key.keysym.sym == SDLK_DOWN) {
		  handleinput(7);
		} else if (event.key.keysym.unicode <= 255) {
          keyHit = event.key.keysym.unicode;
		}
        break;
      case SDL_VIDEORESIZE:
        /* Why are there events when there is no resize? */
        if (XMax != (event.resize.w / scaling - 1) ||
            YMax != (event.resize.h / scaling - 1)) {
		XMax = event.resize.w / scaling - 1;
		YMax = event.resize.h / scaling - 1;
		SKIP = TRUE;
		disp_init();
        }
		break;
      case SDL_QUIT:
		  //abort();
		handleinput(3);
		break;
      default:
        break;
    }
  }
  
  if (keyHit != 0) handleInputChar(keyHit);
}

void disp_init()
{
  Uint32 videoflags = SDL_HWSURFACE | SDL_DOUBLEBUF |
                      (fullscreen ? SDL_FULLSCREEN : SDL_RESIZABLE);
  static int inited = 0;
  static int nativedepth = 0;
  int usedepth;
  
  if (!inited) {
    const SDL_VideoInfo *vi;

    /* Initialize SDL */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
      disp_SDLFatal("initializing video subsystem");
    }

    vi = SDL_GetVideoInfo();
    nativedepth = vi->vfmt->BitsPerPixel;

    SDL_WM_SetCaption("Acidwarp","acidwarp");

    SDL_EnableUNICODE(1);

    /* XMax = 319;
    YMax = 199;	 */
  }

  /* If resizing, there may be old stuff which needs to be freed */
  /* If there was a separate graphics buffer, free it. */
  if (!disp_DrawingOnSurface && buf_graf != NULL) {
    free(buf_graf);
  }
  /* Free secondary surface */
  if (surface != NULL && surface != screen) {
    SDL_FreeSurface(surface);
  }
  /* No need to ever free the screen surface from SDL_SetVideoMode() */
  
  SDL_ShowCursor(!fullscreen);

#ifdef HAVE_PALETTE
  if (fullscreen || nativedepth == 8) {
    disp_UsePalette = 1;
    usedepth = 8;
    videoflags |= SDL_HWPALETTE;
  } else {
    disp_UsePalette = 0;
    usedepth = nativedepth;
    videoflags |= SDL_ANYFORMAT;
  }
#else
  usedepth = nativedepth;
  videoflags |= SDL_ANYFORMAT;
#endif

  // If going back to windowed mode, restore window size
  if (!fullscreen) {
    scaling = 1;
    if (winwidth != 0) {
	  XMax = winwidth;
	  YMax = winheight;
	  SKIP = TRUE;
      winwidth = 0;	  
    }
  } else {
    /* Get available fullscreen modes */
    SDL_Rect **modes = SDL_ListModes(0, videoflags);

    if(modes == (SDL_Rect **)-1){
      /* All resolutions ok */
      scaling = 1;
    } else {
      // Full screen should really fill the whole screen
      // Find video mode with closest number of pixels
      int newwidth = 0;
	  int newheight = 0;
	  int curdiff;
	  int bestdiff = -1;
	  int curpix = (XMax+1) * (YMax+1);
	  int i, j;
      for(i=0;modes[i];i++) {
	    for (j=1;j<=2; j++) { // try out scaling
	      curdiff = modes[i]->w * modes[i]->h / (j * j) - curpix;
	      if ((curdiff) < 0)
	        curdiff = -curdiff;
          if (bestdiff == -1 || curdiff < bestdiff || 
	          (curdiff == bestdiff && j < scaling)) {
            scaling = j;
   	        newwidth = modes[i]->w / j;
	        newheight = modes[i]->h / j;
	        bestdiff = curdiff;
		  }	  
	    }
	  }
	  
	  if (newwidth != 0 && (newwidth != XMax+1 || newheight != YMax+1)) {
	    winwidth = XMax;
	    winheight = YMax;
		XMax = newwidth - 1;
		YMax = newheight - 1;
 	    SKIP = TRUE;
      }
	}
  }

  /* The screen is a destination for SDL_BlitSurface() copies.
   * Nothing is ever directly drawn here.
   */
  screen = SDL_SetVideoMode((XMax+1)*scaling, (YMax+1)*scaling,
                            usedepth, videoflags);
  if (!screen) disp_SDLFatal("setting video mode");

#ifdef HAVE_PALETTE
  if (disp_UsePalette) {
    /* When using a real palette, buf_graf is used instead. */
    surface = screen;
  } else
#endif
  {
    /* Create 8 bit surface to draw into. This is needed if pixel
     * formats differ or to respond to SDL_VIDEOEXPOSE events.
     */
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                   (XMax+1)*scaling, (YMax+1)*scaling,
                                   8, 0, 0, 0, 0);
  }

  if (!surface) disp_SDLFatal("creating secondary surface");

  if (scaling == 1
#ifdef HAVE_PALETTE
      && !disp_UsePalette
#endif
      ) {
    disp_DrawingOnSurface = 1;
    if (!SDL_MUSTLOCK(surface)) {
      buf_graf = surface->pixels;
      buf_graf_stride = surface->pitch;
    }
  } else {
    disp_DrawingOnSurface = 0;
    buf_graf = malloc ((XMax + 1) * (YMax + 1));
    buf_graf_stride = XMax + 1;
    /* Clearing is only needed for the initial logo */
    if (!inited) memset(buf_graf, 0, (XMax + 1) * (YMax + 1));
  }

  inited = 1;  
}