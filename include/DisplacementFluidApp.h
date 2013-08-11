/***********************************************************************
 
 Copyright (c) 2013 Thomas Sanchez Legeling
 *** Codigo Generativo ***
  codigogenerativo.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * ***********************************************************************/ 
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
#include "cinder/Arcball.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include "ciMsaFluidSolver.h"
#include "ciMsaFluidDrawerGl.h"

#include "Particle.h"
#include "ParticleSystem.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;

class msaFluidParticlesApp : public AppBasic {
 public:
	void	setup();
 
	void	fadeToColor( float r, float g, float b, float speed );
	void	addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce );
	void	keyDown( KeyEvent event );
	void	mouseMove( MouseEvent event );
	void	mouseDrag( MouseEvent event );
	void	mouseDown( MouseEvent event);
	void	resize( );
	
	void	update();
	void	draw();

	void	loadShader();

	//ROTATION
	Arcball				mArcball;
	ci::Vec2i			mInitialMouseDown;
	ci::Vec2i			mCurrentMouseDown;

	//SHADER
	gl::GlslProgRef		mShader;
	
	int					fluidCellsX;
	bool				resizeFluid;
	bool				drawFluid;
	bool				drawParticles;
	bool				renderUsingVA;
	
	ciMsaFluidSolver	fluidSolver;
	ciMsaFluidDrawerGl	fluidDrawer;	
	ParticleSystem		particleSystem;
	
	Vec2i				pMouse;

	gl::Fbo				mFbo;

	void				renderSceneToFbo();

	float				width;
	float				height;

	bool				drawFluidTex;
};