/*
 *  Audio user interface
 *  Copyright (C) 2007 Andreas �man
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <libglw/glw.h>

#include "input.h"
#include "layout/layout.h"
#include "showtime.h"
#include "audio_sched.h"
#include "audio_ui.h"

static float audio_alpha;
static int show_audio;
static glw_t *audio_widget;

void
audio_render(float alpha)
{
  glw_rctx_t rc;
  asched_t *as = &audio_scheduler;

  memset(&rc, 0, sizeof(rc));
  rc.rc_aspect = 16.0 / 9.0 * 20.0f;

  if(show_audio > 0)
    show_audio--;

  audio_alpha = (audio_alpha * 15 + 
		 (show_audio || as->as_mute ? 1 : 0)) / 16.0f;

  if(audio_alpha < 0.01 || audio_widget == NULL)
    return;

  rc.rc_alpha = audio_alpha * alpha;

  glPushMatrix();

  glTranslatef(0.0, 0.5, 2.0f);
  glScalef(0.8 * 0.6, 0.6 * 0.04, 1.0f);
  
  glw_render(audio_widget, &rc);

  glPopMatrix();
}

void
audio_layout(void)
{
  glw_rctx_t rc;
  memset(&rc, 0, sizeof(rc));
  rc.rc_aspect = 16.0 / 9.0 * 20.0f;

  glw_layout(audio_widget, &rc);
}

void
audio_ui_vol_changed(void)
{
  show_audio = 100;
}



/*****************************************************************************
 *
 * Master volume
 *
 */

static int 
audio_mastervol_bar_callback(glw_t *w, void *opaque, glw_signal_t signal, ...)
{
  asched_t *as = opaque;

  switch(signal) {
    return 0;

  case GLW_SIGNAL_PREPARE:
    w->glw_extra = GLW_LP(3, w->glw_extra, as->as_mastervol);

    if(as->as_mute)
      glw_set(w, GLW_ATTRIB_COLOR, GLW_COLOR_LIGHT_RED, NULL);
    else
      glw_set(w, GLW_ATTRIB_COLOR, GLW_COLOR_LIGHT_GREEN, NULL);

    return 0;

  default:
    return 0;
  }
}


static int 
audio_mastervol_txt_callback(glw_t *w, void *opaque, glw_signal_t signal, ...)
{
  asched_t *as = opaque;
  char buf[30];

  switch(signal) {
  case GLW_SIGNAL_PREPARE:
    if(as->as_mute)
      snprintf(buf, sizeof(buf), "Master volume: Muted");
    else
      snprintf(buf, sizeof(buf), "Master volume: %d%%", 
	       (int)(as->as_mastervol * 100));
    glw_set(w, GLW_ATTRIB_CAPTION, buf, NULL);
    return 0;

  default:
    return 0;
  }
}



void
audio_widget_make(asched_t *as)
{
  glw_t *w, *z;


  w = glw_create(GLW_BITMAP,
		 GLW_ATTRIB_FILENAME, "icon://plate-wide.png",
		 GLW_ATTRIB_FLAGS, GLW_NOASPECT,
		 NULL);

  audio_widget = w;

  z = glw_create(GLW_CONTAINER_Z,
		 GLW_ATTRIB_PARENT, w,
		 NULL);
		 
  glw_create(GLW_BAR,
	     GLW_ATTRIB_PARENT, z,
	     GLW_ATTRIB_ALIGNMENT, GLW_ALIGN_CENTER,
	     GLW_ATTRIB_SIGNAL_HANDLER, audio_mastervol_bar_callback, as, 0,
	     NULL);

  glw_create(GLW_TEXT_BITMAP,
	     GLW_ATTRIB_PARENT, z,
	     GLW_ATTRIB_ALIGNMENT, GLW_ALIGN_CENTER,
	     GLW_ATTRIB_SIGNAL_HANDLER, audio_mastervol_txt_callback, as, 0,
	     NULL);
}
