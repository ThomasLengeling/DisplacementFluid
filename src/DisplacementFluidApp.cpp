#include "DisplacementFluidApp.h"

void msaFluidParticlesApp::setup()
{
	console() << "ciMSAFluid Demo | (c) 2009 Mehmet Akten | www.memo.tv" << std::endl;
	
	// setup fluid stuff
	fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(0.004).setDeltaT(0.5).setVisc(0.00019).setColorDiffusion(0);
	fluidDrawer.setup( &fluidSolver );
	particleSystem.setFluidSolver( &fluidSolver );
	
	fluidCellsX			= 150;
	
	drawFluid			= true;
	drawParticles		= true;
	renderUsingVA		= true;

	width				= 1024;
	height				= 768;
	
	setFrameRate( 60.0f );
	setWindowSize(width, height);

	
	pMouse = getWindowCenter();
	resizeFluid			= true;
	


	gl::enableAlphaBlending();

	gl::Fbo::Format format;
	format.setSamples( 8 );
	//format.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	mFbo = gl::Fbo(width, height, format);

	drawFluidTex = false;

	loadShader();
}

void msaFluidParticlesApp::update()
{
	if( resizeFluid ) {
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}
	
	fluidSolver.update();

	renderSceneToFbo();
}

void msaFluidParticlesApp::draw()
{
	gl::clear( Color( 1.0f, 1.0f, 1.0f ) );


	CameraPersp cam( getWindowWidth(), getWindowHeight(), 60.0f );
	cam.setPerspective( 60, getWindowAspectRatio(), 1, 1500 );
	cam.lookAt( Vec3f( 2.6f, 1.6f, -2.6f ), Vec3f::zero() );
	gl::setMatrices( cam );
	gl::rotate( mArcball.getQuat() );

	// set the viewport to match our window
	gl::setViewport( getWindowBounds() );

	gl::enableDepthRead();
	gl::enableDepthWrite();

	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK);

	//mFbo.bindTexture();
	gl::Texture txFluid = mFbo.getTexture(0);

	
	mShader->bind();
	txFluid.enableAndBind();
	mShader->uniform( "displacementMap", 0 );
	mShader->uniform( "colorMap", 0 );
	gl::drawSphere(Vec3f::zero(), 1.0, 254);
	txFluid.unbind();
	mShader->unbind();
	//mFbo.unbindTexture();

	gl::disable(GL_CULL_FACE);
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::setMatricesWindow( getWindowSize());
	if(drawFluidTex){
		glEnable( GL_TEXTURE_2D );
		txFluid.setFlipped(true);
		gl::draw( txFluid, Rectf( 0, 0, 256, 256));
		glDisable(GL_TEXTURE_2D);
	}
}


void msaFluidParticlesApp::resize()
{
	particleSystem.setWindowSize( app::getWindowSize());
	width  = app::getWindowWidth();
	height = app::getWindowHeight();

	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 150 );

	resizeFluid = true;
}

void msaFluidParticlesApp::keyDown( KeyEvent event )
{ 
    switch( event.getChar() ) {
		case 'a':
			drawFluid = !drawFluid;
		break;
		case 'd':
			drawFluidTex = !drawFluidTex;
		break;
		case 'f':
			setFullScreen( ! isFullScreen() );
		break;
		case ' ':
			fluidSolver.randomizeColor();
		break;
		case 'p':
			drawParticles = ! drawParticles;
		break;
		case 'b': {
			Timer timer;
			timer.start();
			const int ITERS = 1000;
			for( int i = 0; i < ITERS; ++i )
				fluidSolver.update();
			timer.stop();
			console() << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
		}
		break;
		case 's':
			writeImage( getHomeDirectory() / "cinder" / "saveImage_" / ( toString( getElapsedFrames() ) + ".png" ), copyWindowSurface() );
			break;
    }
}

void msaFluidParticlesApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event.getPos() );
	mCurrentMouseDown = mInitialMouseDown = event.getPos();
}

void msaFluidParticlesApp::mouseMove( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = event.getPos();
}

void msaFluidParticlesApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() ) / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, false, true );
	pMouse = event.getPos();

	mArcball.mouseDrag( event.getPos() );
	mCurrentMouseDown = event.getPos();
}

void msaFluidParticlesApp::loadShader()
{
	try {
		mShader = gl::GlslProg::create( loadResource( RES_DISPLACEMENT_VERT ), loadResource( RES_DISPLACEMENT_FRAG ) );
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << std::endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << std::endl;
	}
}

void msaFluidParticlesApp::renderSceneToFbo()
{
	mFbo.bindFramebuffer();

	if( drawFluid ) {
		glColor3f(1, 1, 1);
		fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());
	}
	if( drawParticles )
		particleSystem.updateAndDraw( drawFluid );

	mFbo.unbindFramebuffer();
}

void msaFluidParticlesApp::fadeToColor( float r, float g, float b, float speed )
{
	glColor4f( r, g, b, speed );
	gl::drawSolidRect( getWindowBounds() );
}

// add force and dye to fluid, and create particles
void msaFluidParticlesApp::addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce )
{
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if( speed > 0 ) {
		pos.x = constrain( pos.x, 0.0f, 1.0f );
		pos.y = constrain( pos.y, 0.0f, 1.0f );
		
        const float colorMult = 100;
        const float velocityMult = 30;
		
		if( addColor ) {
			Color drawColor( CM_HSV, ( getElapsedFrames() % 360 ) / 360.0f, 1, 1 );
			
			fluidSolver.addColorAtPos( pos, drawColor * colorMult );

			if( drawParticles )
				particleSystem.addParticles( pos * Vec2f( getWindowSize() ), 10 );
		}
		
		if( addForce )
			fluidSolver.addForceAtPos( pos, vel * velocityMult );
		
		if( ! drawFluid && getElapsedFrames()%5==0 )
			fadeToColor( 0, 0, 0, 0.1f );
    }
}

CINDER_APP_BASIC( msaFluidParticlesApp, RendererGl )
