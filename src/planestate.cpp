////////////////////////////////////////////////////////////////////////////
//
// Planestate Screensaver for XBox Media Center
// Copyright (c) 2005 Joakim Eriksson <je@plane9.com>
//
////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
////////////////////////////////////////////////////////////////////////////
//
// This	screensaver is based on my old windows planestate screensaver that
// can be found	at http://www.plane9.com.
//
// The screensaver uses 'configurations'. A configuration is a specific
// setting on the currently around 210 diffrent values that can be set.
// These configurations gets picked at run time according to a probability
// factor so you might need to run the screensaver a few times before you
// have seen all of them.
//
// The windows screensaver has a real configuration gui so if you want to
// create new configurations I suggest you use that screensaver then write
// down all values and add them in here
//
////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "planestate.h"
#include <SOIL/SOIL.h>
#include <memory.h>
#include <vector>

typedef	struct	TRenderVertex
{
	CVector		pos;
	CRGBA col;
	f32			u, v;
} TRenderVertex;

#define NUMCFGVALUES	(3*6+2*2+1)

// Animation configurations.
// Tells how we should animate the values. How quickly we should move from one value
// to the next and for how long we should stay at that value before going to a new value

int		gNumPlanes[NUMCFGS] = { 500, 500, 800, 150 };
bool	gBillboard[NUMCFGS] = { false, false, true, false };
f32		gDistToCam[NUMCFGS] = { 900.0f,1000.0f, 400.0f, 400.0f };

CAnimValueCfg	gAnimCfg[NUMCFGS][NUMCFGVALUES] =
{
	{
		// Based on 'Wonder' Frame:0 as found in the windows planestate screensaver
		// Value min,max,mode		Delay. Min,max,mode		Interpolation time.	Min,max,mode	
		{75.0f,	75.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane Size X
		{10.0f,	10.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane Size Y

		{-200.0f, 200.0f, AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Pivot	Point X
		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Pivot	Point Y
		{-62.0f, 50.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Pivot	Point Z

		{1.4f,	1.4f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane-Plane Distance

		{ 0.0f,	360.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Rotation X
		{ 0.0f,	360.0f,	AM_PINGPONG,0.0f, 0.0f,	AM_NONE,	10.0f,20.0f,AM_RAND	},		//	Plane Rotation Y
		{ 0.0f,	360.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Rotation Z

		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane Rotation Around Center Axis X
		{71.8f,	71.8f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane Rotation Around Center Axis Y
		{ 0.2f,	0.6f, AM_RAND,		0.0f, 6.0f,	AM_RAND,	8.0f,40.0f,AM_RAND	},		//	Plane Rotation Around Center Axis Z

		{ 0.0f,	0.2f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Red
		{ 0.0f,	0.2f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Green
		{ 0.0f,	0.2f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Blue

		{90.0f,	90.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Center Axis Rotation X
		{ 0.0f,	 0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Center Axis Rotation Y
		{ 0.0f,	 0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Center Axis Rotation Z

		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	6.0f,60.0f,	AM_RAND	},		//	Plane Distance To Center Axis X
		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	6.0f,60.0f,	AM_RAND	},		//	Plane Distance To Center Axis Y
		{-35.0f, 94.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,60.0f,	AM_RAND	},		//	Plane Distance To Center Axis Z

		{-10.0f, 10.0f, AM_RAND,	0.0f, 6.0f,	AM_RAND,	20.0f,60.0f, AM_RAND },		// Camera Rot X
		{-10.0f, 10.0f, AM_RAND,	0.0f, 6.0f,	AM_RAND,	20.0f,60.0f, AM_RAND },		// Camera Rot Y
	},
	{
		// Value min,max,mode		Delay. Min,max,mode		Interpolation time.	Min,max,mode	
		{10.0f,	10.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane Size X
		{62.0f,	62.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane Size Y

		{-200.0f, 200.0f, AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Pivot	Point X
		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Pivot	Point Y
		{-62.0f, 50.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Pivot	Point Z

		{1.5f,	1.5f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane-Plane Distance

		{ 0.0f,	360.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Rotation X
		{ 0.0f,	360.0f,	AM_PINGPONG,0.0f, 0.0f,	AM_NONE,	10.0f,20.0f,AM_RAND	},		//	Plane Rotation Y
		{ 0.0f,	360.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Rotation Z

		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Plane Rotation Around Center Axis X
		{51.4f,	51.4f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	10.0f,20.0f,AM_RAND	},		//	Plane Rotation Around Center Axis Y
		{ 0.2f,	0.6f, AM_RAND,		0.0f, 6.0f,	AM_RAND,	8.0f,40.0f,AM_RAND	},		//	Plane Rotation Around Center Axis Z

		{ 0.0f,	0.2f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Red
		{ 0.0f,	0.2f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Green
		{ 0.0f,	0.2f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Blue

		{90.0f,	90.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Center Axis Rotation X
		{ 0.0f,	 0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Center Axis Rotation Y
		{ 0.0f,	 0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	12.0f,40.0f,AM_RAND	},		//	Center Axis Rotation Z

		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Distance To Center Axis X
		{ 0.0f,	0.0f, AM_NONE,		0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Distance To Center Axis Y
		{-35.0f, 94.0f,	AM_RAND,	0.0f, 6.0f,	AM_RAND,	6.0f,40.0f,	AM_RAND	},		//	Plane Distance To Center Axis Z

		{-10.0f, 10.0f, AM_RAND,	0.0f, 6.0f,	AM_RAND,	20.0f,60.0f, AM_RAND },		// Camera Rot X
		{-10.0f, 10.0f, AM_RAND,	0.0f, 6.0f,	AM_RAND,	20.0f,60.0f, AM_RAND },		// Camera Rot Y
	},
	{
		// Based on 'Round and around' Frame:0 as found in the windows planestate screensaver
		// Value min,max,mode		Delay. Min,max,mode		Interpolation time.	Min,max,mode	
		{10.0f,	10.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Size X
		{10.0f,	10.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Size Y

		{ 5.0f,  5.0f, AM_NONE,		0.0f, 1.0f,	AM_RAND,	1.0f,10.0f,	AM_RAND	},		//	Plane Pivot	Point X
		{-78.0f, 78.0f, AM_RAND,	0.0f, 1.0f,	AM_RAND,	1.0f,10.0f,	AM_RAND	},		//	Plane Pivot	Point Y
		{-78.0f, 78.0f,	AM_RAND,	0.0f, 1.0f,	AM_RAND,	1.0f,10.0f,	AM_RAND	},		//	Plane Pivot	Point Z

		{-0.2f,	1.0f, AM_RAND,		0.0f, 1.0f,	AM_RAND,	1.0f,10.0f,	AM_RAND	},		//	Plane-Plane Distance

		{178.0f, 178.0f, AM_NONE,	0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Rotation X
		{178.0f, 178.0f, AM_NONE,	0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Rotation Y
		{178.0f, 178.0f, AM_NONE,	0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Rotation Z

		{ 0.0f,	0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Rotation Around Center Axis X
		{ 0.0f,	0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Rotation Around Center Axis Y
		{14.0f,14.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Rotation Around Center Axis Z

		{ 0.0f,	0.15f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Red
		{ 0.0f,	0.15f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Green
		{ 0.0f,	0.15f, AM_RAND,		0.0f, 4.8f,	AM_RAND,	1.6f, 3.3f,	AM_RAND	},		//	Color Blue

		{ 0.0f,360.0f, AM_RAND,		0.0f, 1.0f,	AM_RAND,	10.0f,20.0f, AM_RAND },		//	Center Axis Rotation X
		{ 0.0f,360.0f, AM_RAND,		0.0f, 1.0f,	AM_RAND,	10.0f,20.0f, AM_RAND },		//	Center Axis Rotation Y
		{ 0.0f,360.0f, AM_RAND,		0.0f, 1.0f,	AM_RAND,	10.0f,20.0f, AM_RAND },		//	Center Axis Rotation Z

		{-4.0f,-4.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Distance To Center Axis X
		{-2.0f,-2.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Distance To Center Axis Y
		{-2.5f,-2.5f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Distance To Center Axis Z

		{  0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	10.0f,30.0f, AM_RAND },		// Camera Rot X
		{  0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	10.0f,30.0f, AM_RAND },		// Camera Rot Y
	},
	{
		// Based on 'Beyond the stars' as found in the windows planestate screensaver
		// Value min,max,mode		Delay. Min,max,mode		Interpolation time.	Min,max,mode	
		{200.0f,200.0f, AM_NONE,	0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Size X
		{ 5.0f,	  5.0f, AM_NONE,	0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Size Y

		{ 0.0f,  0.0f, AM_NONE,		0.0f, 1.0f,	AM_RAND,	1.0f,10.0f,	AM_RAND	},		//	Plane Pivot	Point X
		{ 0.0f,  0.0f, AM_NONE,		0.0f, 1.0f,	AM_RAND,	1.0f,10.0f,	AM_RAND	},		//	Plane Pivot	Point Y
		{ 0.0f,  0.0f, AM_NONE,		0.0f, 1.0f,	AM_RAND,	1.0f,10.0f,	AM_RAND	},		//	Plane Pivot	Point Z

		{-10.0f,10.0f, AM_RAND,		0.0f, 0.0f,	AM_NONE,	4.0f,8.0f,	AM_RAND	},		//	Plane-Plane Distance

		{0.0f, 360.0f,AM_RAND,		0.0f, 3.0f,	AM_RAND,	4.0f,8.0f,	AM_RAND	},		//	Plane Rotation X
		{ 0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Rotation Y
		{ 0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Rotation Z

		{ 0.0f,	0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Rotation Around Center Axis X
		{ 7.2f,	7.2f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	4.0f,8.0f,	AM_RAND	},		//	Plane Rotation Around Center Axis Y
		{ 0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	6.0f,20.0f,	AM_RAND	},		//	Plane Rotation Around Center Axis Z

		{ 0.0f,	0.3f, AM_RAND,		0.0f, 2.8f,	AM_RAND,	1.1f, 2.3f,	AM_RAND	},		//	Color Red
		{ 0.0f,	0.3f, AM_RAND,		0.0f, 2.8f,	AM_RAND,	1.1f, 2.3f,	AM_RAND	},		//	Color Green
		{ 0.0f,	0.3f, AM_RAND,		0.0f, 2.8f,	AM_RAND,	1.1f, 2.3f,	AM_RAND	},		//	Color Blue

		{ 0.0f,360.0f, AM_RAND,		0.0f, 1.0f,	AM_RAND,	4.0f,10.0f,	AM_RAND	},		//	Center Axis Rotation X
		{ 0.0f,360.0f, AM_RAND,		0.0f, 1.0f,	AM_RAND,	4.0f,10.0f,	AM_RAND	},		//	Center Axis Rotation Y
		{ 0.0f,360.0f, AM_RAND,		0.0f, 1.0f,	AM_RAND,	4.0f,10.0f,	AM_RAND	},		//	Center Axis Rotation Z

		{ 0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Distance To Center Axis X
		{ 0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Distance To Center Axis Y
		{ 0.0f, 0.0f, AM_NONE,		0.0f, 3.0f,	AM_RAND,	3.0f,20.0f,	AM_RAND	},		//	Plane Distance To Center Axis Z

		{-10.0f, 10.0f, AM_RAND,	0.0f, 3.0f,	AM_RAND,	10.0f,30.0f, AM_RAND },		// Camera Rot X
		{-10.0f, 10.0f, AM_RAND,	0.0f, 3.0f,	AM_RAND,	10.0f,30.0f, AM_RAND },		// Camera Rot Y
	}
};

////////////////////////////////////////////////////////////////////////////
//
CPlanestate::CPlanestate(f32 cfgProbability[NUMCFGS]) :
	m_ColAnim(0, CRGBA(0.0f, 0.0f,	0.0f, 1.0f)), m_RotAnim(1, CVector(0.0f, 0.0f, 0.0f)), m_CADAnim(2,	CVector(0.0f, 0.0f,	0.0f)),
	m_PPPAnim(3, CVector(0.0f,	0.0f, 0.0f)), m_CARAnim(4, CVector(0.0f, 0.0f, 0.0f)), m_CRAnim(5, CVector(0.0f, 0.0f, 0.0f)),
	m_PSAnim(6, CVector(0.0f, 0.0f, 0.0f)), m_PPDAnim(7, 0), m_CMRAnim(8, CVector(0.0f, 0.0f, 0.0f)), m_texture(0)
{
  m_PlaneYDelta =	1.0f;
  m_Size.Set(10.0f, 10.0f, 0.0f);
  m_CenterAxisRot.Zero();

  // Pick a configuration to show.
  m_ConfigNr = Rand(NUMCFGS);
  f32 rand = RandFloat();
  for (int i=0; i<NUMCFGS; i++)
  {
    rand -= cfgProbability[i];
    if (rand <= 0.0f)
    {
      m_ConfigNr = i;
      break;
    }
  }

  // Setup all the values	that will get animated
  CFloatAnimator*	floatAnims[] =
  {
    &m_PSAnim.m_Values[0],	&m_PSAnim.m_Values[1],
    &m_PPPAnim.m_Values[0],	&m_PPPAnim.m_Values[1],	&m_PPPAnim.m_Values[2],	
    &m_PPDAnim,
    &m_RotAnim.m_Values[0],	&m_RotAnim.m_Values[1],	&m_RotAnim.m_Values[2],	
    &m_CARAnim.m_Values[0],	&m_CARAnim.m_Values[1],	&m_CARAnim.m_Values[2],	
    &m_ColAnim.m_Values[0],	&m_ColAnim.m_Values[1],	&m_ColAnim.m_Values[2],
    &m_CRAnim.m_Values[0],	&m_CRAnim.m_Values[1],	&m_CRAnim.m_Values[2],	
    &m_CADAnim.m_Values[0],	&m_CADAnim.m_Values[1],	&m_CADAnim.m_Values[2],	
    &m_CMRAnim.m_Values[0],	&m_CMRAnim.m_Values[1],
  };
  CAnimValueCfg*	cfg	= gAnimCfg[m_ConfigNr];
  for	(int vNr=0;	vNr<NUMCFGVALUES; vNr++)
  {
    floatAnims[vNr]->SetMinMax(	 cfg[vNr].m_MinValue, cfg[vNr].m_MaxValue);
    floatAnims[vNr]->m_AnimMode	= cfg[vNr].m_AnimMode;
    floatAnims[vNr]->SetMinMaxITime(cfg[vNr].m_MinITime, cfg[vNr].m_MaxITime, cfg[vNr].m_ITimeAM);
    floatAnims[vNr]->SetMinMaxDelay(cfg[vNr].m_MinDelay, cfg[vNr].m_MaxDelay, cfg[vNr].m_DelayAM);
  }

  m_NumPlanes			= gNumPlanes[m_ConfigNr];
  m_PlaneUpdateDelay	= 0.015f;
  m_Planes			= new CPSPlane[m_NumPlanes];

  // Perform a number	of updates so all planes get away from thier inital position
  for	(int i=0; i<m_NumPlanes*2; i++)
  {
    UpdatePlane(1.0f/50.0f);
    m_Background.Update(1.0f/50.0f);
  }
}

////////////////////////////////////////////////////////////////////////////
//
CPlanestate::~CPlanestate()
{
  SAFE_DELETE_ARRAY(m_Planes);
}

////////////////////////////////////////////////////////////////////////////
//
bool	CPlanestate::RestoreDevice(CRenderD3D* render)
{
  if (!CreatePlaneTexture(render))
    return false;
  m_Background.RestoreDevice(render);
  return true;
}

////////////////////////////////////////////////////////////////////////////
//
void	CPlanestate::InvalidateDevice(CRenderD3D* render)
{
  m_Background.InvalidateDevice(render);
  if (m_texture)
    glDeleteTextures(1, &m_texture);
}

////////////////////////////////////////////////////////////////////////////
//
void		CPlanestate::Update(f32 dt)
{
  m_Background.Update(dt);

  UpdatePlane(dt);

  // Create the transformation matrix for each plane
  CMatrix	centerAxis, tmp, tmp2, cAxisRot, pivotRot;
  CVector	tmpPos;
  centerAxis.Identity();
  centerAxis.Rotate(m_CenterAxisRot.x, m_CenterAxisRot.y, m_CenterAxisRot.z);
  CPSPlane*	plane =	m_Planes;
  for	(int pnr=0;	pnr<m_NumPlanes; pnr++, plane++)
  {
    f32			rotScale = (f32)pnr+1.0f;
    cAxisRot.Rotate(m_PlaneCAxisRot.x*rotScale,	m_PlaneCAxisRot.y*rotScale,	m_PlaneCAxisRot.z*rotScale);

    plane->m_Transform.Identity();
    plane->m_Transform.Scale(plane->m_Scale.x, plane->m_Scale.y, 1.0);
    pivotRot.Rotate(plane->m_Rot.x,	plane->m_Rot.y,	plane->m_Rot.z);

    tmpPos = pivotRot *	plane->m_PivotPos;
    tmp.Multiply(pivotRot, plane->m_Transform);

    tmp._41 = tmpPos.x+plane->m_CenterAxisDist.x;
    tmp._42 = tmpPos.y+plane->m_CenterAxisDist.y+(f32)pnr*m_PlaneYDelta-((f32)m_NumPlanes*m_PlaneYDelta*0.5f);
    tmp._43 = tmpPos.z+plane->m_CenterAxisDist.z;

    tmp2.Multiply(centerAxis, cAxisRot);
    plane->m_Transform.Multiply(tmp2, tmp);

    if (gBillboard[m_ConfigNr])
    {
      // Billboarded so kill the rotation
      plane->m_Transform._12 = plane->m_Transform._13 = 0;	plane->m_Transform._11 = plane->m_Scale.x;
      plane->m_Transform._21 = plane->m_Transform._23 = 0;	plane->m_Transform._22 = plane->m_Scale.y;
      plane->m_Transform._31 = plane->m_Transform._32 = 0;	plane->m_Transform._33 = 0.0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////
// Update the animation	values and cycles the planes
//
void	CPlanestate::UpdatePlane(f32 dt)
{
  CPSPlane* plane	= &m_Planes[0];

  static f32 delay = 0.0f;
  delay -= dt;
  if (delay <	0.0f)
  {
    // Move	all	the	plane data up one step
    for	(int i=m_NumPlanes-1; i>0; i--)
    {
      m_Planes[i].Copy(m_Planes[i-1]);
    }
    delay =	m_PlaneUpdateDelay;		// NOTE: We	shouldn't just clear this. We will get uneven times. However it will probably not be noticible in this case

    plane->m_Col = m_ColAnim.GetValue();
    plane->m_Rot = m_RotAnim.GetValue();
    plane->m_CenterAxisDist	= m_CADAnim.GetValue();
    plane->m_PivotPos =	m_PPPAnim.GetValue();
    plane->m_Scale = m_PSAnim.GetValue();
  }

  // Animate all values
  m_PPDAnim.Update(dt);
  m_PSAnim.Update(dt);
  m_ColAnim.Update(dt);
  m_RotAnim.Update(dt);
  m_CADAnim.Update(dt);
  m_PPPAnim.Update(dt);
  m_CARAnim.Update(dt);
  m_CRAnim.Update(dt);
  m_CMRAnim.Update(dt);

  m_PlaneYDelta	= m_PPDAnim.GetValue();
  m_CenterAxisRot = m_CRAnim.GetValue();
  m_PlaneCAxisRot	= m_CARAnim.GetValue();
}

////////////////////////////////////////////////////////////////////////////
//
bool		CPlanestate::Draw(CRenderD3D* render)
{
//  D3DXMATRIX mProjection;
//  D3DXMatrixPerspectiveFovLH(	&mProjection, DEGTORAD(	60.0f ), (f32)render->m_Width / (f32)render->m_Height, 0.1f, 2000.0f );
//  d3dDevice->SetTransform( D3DTS_PROJECTION, &mProjection );
//  CMatrix		tmp;
//  tmp.Identity();
//  d3dDevice->SetTransform(D3DTS_VIEW, (D3DXMATRIX*)&tmp);

  m_Background.Draw(render);

  // Fill	in the vertex buffers with the plane vertices
  std::vector<TRenderVertex> verts(m_NumPlanes*4);
  TRenderVertex* vert	= &verts[0];
  for	(int pNr=0;	pNr<m_NumPlanes; pNr++)
  {
    CPSPlane* plane	= &m_Planes[pNr];
    CRGBA col	= plane->m_Col;
    vert->pos =	plane->m_Transform*CVector(-1.0f, 1.0f,	0.0f);	vert->u	= 0.0f;			vert->v	= 0.0f;			vert->col =	col; vert++;
    vert->pos =	plane->m_Transform*CVector(-1.0f,-1.0f,	0.0f);	vert->u	= 1.0;	vert->v	= 0.0f;			vert->col =	col; vert++;
    vert->pos =	plane->m_Transform*CVector(	1.0f, 1.0f,	0.0f);	vert->u	= 0.0f;			vert->v	= 1.0;	vert->col =	col; vert++;
    vert->pos =	plane->m_Transform*CVector(	1.0f,-1.0f,	0.0f);	vert->u	= 1.0;	vert->v	= 1.0;	vert->col =	col; vert++;
  }

  glBegin(GL_TRIANGLE_STRIP);
  for (size_t i=0;i<m_NumPlanes*4;++i)
  {
    glColor3f(verts[i].col.r/255.0,
              verts[i].col.g/255.0,
              verts[i].col.b/255.0);
    glTexCoord2f(verts[i].u, verts[i].v);
    glVertex3f(verts[i].pos.x, verts[i].pos.y, verts[i].pos.z);
  }
  glEnd();

  return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// Creates the plane texture. It's just a big white blob in the center
//
bool	CPlanestate::CreatePlaneTexture(CRenderD3D* render)
{
  // Create star texture
  unsigned char* data = new unsigned char[256*256*4];

  memset(data, 0, 256*256*4);

  f32		cx = (f32)256/2.0f, cy = (f32)256/2.0f;
  int		pitch	= 256;
  for	(int y=0; y<256;	y++)
  {
    u32*	ptr	= (u32*) data+pitch*y;
    for	(int x=0; x<256; x++)
    {
      f32	dx = cx	- (f32)x, dy = cy - (f32)y;
      f32	brightness = 1.0f -	(sqrt(dx*dx+dy*dy)/((f32)256/2.0f));
      *ptr++ =  CRGBA(brightness, brightness, brightness, 1.0f).RenderColor();
    }
  }

  m_texture = SOIL_create_OGL_texture(data, 256, 256, SOIL_LOAD_RGBA, 0, 0);
  printf("got textureid %u\n", m_texture);

  return true;
}
