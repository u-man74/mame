// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
//============================================================
//
//  drawbgfx.c - BGFX drawer
//
//============================================================

// standard windows headers
#define __STDC_LIMIT_MACROS
#define __STDC_FORMAT_MACROS
#define __STDC_CONSTANT_MACROS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// MAME headers
#include "emu.h"

// MAMEOS headers
#include "window.h"

#include <bgfxplatform.h> 
#include <bgfx.h>

//============================================================
//  PROTOTYPES
//============================================================

// core functions
static void drawbgfx_exit(void);
static int drawbgfx_window_init(win_window_info *window);
static void drawbgfx_window_destroy(win_window_info *window);
static render_primitive_list *drawbgfx_window_get_primitives(win_window_info *window);
static int drawbgfx_window_draw(win_window_info *window, HDC dc, int update);



//============================================================
//  drawbgfx_init
//============================================================

int drawbgfx_init(running_machine &machine, win_draw_callbacks *callbacks)
{
	// fill in the callbacks
	memset(callbacks, 0, sizeof(*callbacks));
	callbacks->exit = drawbgfx_exit;
	callbacks->window_init = drawbgfx_window_init;
	callbacks->window_get_primitives = drawbgfx_window_get_primitives;
	callbacks->window_draw = drawbgfx_window_draw;
	callbacks->window_destroy = drawbgfx_window_destroy;
	return 0;
}



//============================================================
//  drawbgfx_exit
//============================================================

static void drawbgfx_exit(void)
{
}



//============================================================
//  drawbgfx_window_init
//============================================================

static int drawbgfx_window_init(win_window_info *window)
{
	RECT client;
	GetClientRect(window->m_hwnd, &client);

	bgfx::winSetHwnd(window->m_hwnd);
	bgfx::init();
	bgfx::reset(rect_width(&client), rect_height(&client), BGFX_RESET_VSYNC);
	
	// Enable debug text.
	bgfx::setDebug(BGFX_DEBUG_STATS);// BGFX_DEBUG_TEXT);

	window->m_drawdata = window->m_hwnd;
	return 0;
}



//============================================================
//  drawbgfx_window_destroy
//============================================================

static void drawbgfx_window_destroy(win_window_info *window)
{
	// Shutdown bgfx.
	bgfx::shutdown();
	window->m_drawdata = NULL;
}



//============================================================
//  drawbgfx_window_get_primitives
//============================================================

static render_primitive_list *drawbgfx_window_get_primitives(win_window_info *window)
{
	RECT client;
	GetClientRect(window->m_hwnd, &client);
	window->m_target->set_bounds(rect_width(&client), rect_height(&client), window->m_monitor->get_aspect());
	return &window->m_target->get_primitives();	
}



//============================================================
//  drawbgfx_window_draw
//============================================================

static int drawbgfx_window_draw(win_window_info *window, HDC dc, int update)
{
	RECT client;
	GetClientRect(window->m_hwnd, &client);

	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		, 0x00000000
		, 1.0f
		, 0
		);
	// Set view 0 default viewport.
	bgfx::setViewRect(0, 0, 0, rect_width(&client), rect_height(&client));

	// This dummy draw call is here to make sure that view 0 is cleared
	// if no other draw calls are submitted to view 0.
	bgfx::submit(0);

	// Use debug font to print information about this example.
	//bgfx::dbgTextClear();
	//bgfx::dbgTextPrintf(0, 1, 0x4f, "MAME BGFX test");
	//bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Initialization and debug text.");

	window->m_primlist->acquire_lock();

	// now draw
	for (render_primitive *prim = window->m_primlist->first(); prim != NULL; prim = prim->next())
	{
		switch (prim->type)
		{
			/**
			 * Try to stay in one Begin/End block as long as possible,
			 * since entering and leaving one is most expensive..
			 */
			case render_primitive::LINE:
				// check if it's really a point
/*				
				if (((prim->bounds.x1 - prim->bounds.x0) == 0) && ((prim->bounds.y1 - prim->bounds.y0) == 0))
				{
					curPrimitive=GL_POINTS;
				} else {
					curPrimitive=GL_LINES;
				}

				if(pendingPrimitive!=GL_NO_PRIMITIVE && pendingPrimitive!=curPrimitive)
				{
					glEnd();
					pendingPrimitive=GL_NO_PRIMITIVE;
				}

				if ( pendingPrimitive==GL_NO_PRIMITIVE )
				{
							set_blendmode(sdl, PRIMFLAG_GET_BLENDMODE(prim->flags));
				}

				glColor4f(prim->color.r, prim->color.g, prim->color.b, prim->color.a);

				if(pendingPrimitive!=curPrimitive)
				{
					glBegin(curPrimitive);
					pendingPrimitive=curPrimitive;
				}

				// check if it's really a point
				if (curPrimitive==GL_POINTS)
				{
					glVertex2f(prim->bounds.x0+hofs, prim->bounds.y0+vofs);
				}
				else
				{
					glVertex2f(prim->bounds.x0+hofs, prim->bounds.y0+vofs);
					glVertex2f(prim->bounds.x1+hofs, prim->bounds.y1+vofs);
				}*/
				break;

			case render_primitive::QUAD:
/*
				if(pendingPrimitive!=GL_NO_PRIMITIVE)
				{
					glEnd();
					pendingPrimitive=GL_NO_PRIMITIVE;
				}

				glColor4f(prim->color.r, prim->color.g, prim->color.b, prim->color.a);

				set_blendmode(sdl, PRIMFLAG_GET_BLENDMODE(prim->flags));

				texture = texture_update(window, prim, 0);

				
				sdl->texVerticex[0]=prim->bounds.x0 + hofs;
				sdl->texVerticex[1]=prim->bounds.y0 + vofs;
				sdl->texVerticex[2]=prim->bounds.x1 + hofs;
				sdl->texVerticex[3]=prim->bounds.y0 + vofs;
				sdl->texVerticex[4]=prim->bounds.x1 + hofs;
				sdl->texVerticex[5]=prim->bounds.y1 + vofs;
				sdl->texVerticex[6]=prim->bounds.x0 + hofs;
				sdl->texVerticex[7]=prim->bounds.y1 + vofs;

				glDrawArrays(GL_QUADS, 0, 4);
*/
				break;

			default:
				throw emu_fatalerror("Unexpected render_primitive type");
		}
	}

	window->m_primlist->release_lock();	
	// Advance to next frame. Rendering thread will be kicked to 
	// process submitted rendering primitives.
	bgfx::frame();

	return 0;
}