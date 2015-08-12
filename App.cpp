/**
Comp 394 S15 Assignment #2 Ping Pong 3D
**/

#include "App.h"
#include <iostream>
using namespace std;

// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
	(void)argc; (void)argv;
	GApp::Settings settings(argc, argv);

	// Change the window and other startup parameters by modifying the
	// settings class.  For example:
	settings.window.width       = 1280; 
	settings.window.height      = 720;

	return App(settings).run();
}

App::App(const GApp::Settings& settings) : GApp(settings) {
	renderDevice->setColorClearValue(Color3(0.2, 0.2, 0.2));
	renderDevice->setSwapBuffersAutomatically(true);
}


void App::onInit() {
	GApp::onInit();
	// Turn on the developer HUD
	createDeveloperHUD();
	debugWindow->setVisible(false);
	developerWindow->setVisible(false);
	developerWindow->cameraControlWindow->setVisible(false);
	showRenderingStats = false;

	// Setup the camera with a good initial position and view direction to see the table
	activeCamera()->setPosition(Vector3(0,60,230));
	activeCamera()->lookAt(Vector3(0,0,0), Vector3(0,1,0));
	activeCamera()->setFarPlaneZ(-1000);

	position = Vector3(0, 60, -130);
	ballVel = Vector3(0, 0, 52);
	gravity = 9.81;
	time = 0;

	table_normal = Vector3(0, 1, 0);
	net_normal = Vector3(0, 0, 1);
	table_bounce = 0.95;
	net_bounce = 0.65;	
	ballRadius = 2;

	window = GuiWindow::create("Lonely Ping Pong", shared_ptr<GuiTheme>(), Rect2D::xywh(1280/2-100, 10, 100, 100), GuiTheme::NORMAL_WINDOW_STYLE, GuiWindow::REMOVE_ON_CLOSE);
	textBox = window->pane()->addTextBox("", &textToSend);
	textToSend = "Hit the other side for a point";
	App::addWidget(window);	

	points = 0;
}


void App::onUserInput(UserInput *uinput) {

	// This block of code maps the 2D position of the mouse in screen space to a 3D position
	// 20 cm above the ping pong table.  It also rotates the paddle to make the handle move
	// in a cool way.  It also makes sure that the paddle does not cross the net and go onto
	// the opponent's side.
	Vector2 mouseXY = uinput->mouseXY();
	float xneg1to1 = mouseXY[0] / renderDevice->width() * 2.0 - 1.0;
	float y0to1 = mouseXY[1] / renderDevice->height();
	Matrix3 rotZ = Matrix3::fromAxisAngle(Vector3(0, 0, 1), aSin(-xneg1to1));    
	prev_position = paddleFrame.translation;
	paddleFrame = CoordinateFrame(rotZ, Vector3(xneg1to1 * 100.0, 20.0, G3D::max(y0to1 * 137.0 + 20.0, 0.0)));
	Vector3 newPos = paddleFrame.translation;
	
	// This is a weighted average.  Update the velocity to be 10% the velocity calculated 
	// at the previous frame and 90% the velocity calculated at this frame.
	paddleVel = 0.1*paddleVel + 0.9*(newPos - prev_position);

	if (uinput->keyPressed(GKey(' '))) {
		reset();
	}
}

void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
	GApp::onSimulation(rdt, sdt, idt);
	rdt *= 2;
	time += rdt;
	
	Vector3 pos = ballPos(time);


	// Ball comes back
	if (ballPos(time).z <= ballRadius - 137){
		ballVel = bounce(ballVelocity(time), net_normal, net_bounce);
		position = pos;
		time = 0;
		position.z = ballRadius - 137;
		points++;
		textToSend = "Your point!";
	}

	
	if (hitPaddle(pos)) {
		ballVel = paddleVel + bounce(ballVelocity(time), getPaddleNormal(), net_bounce);
		position = pos;
		time = 0;
		position.z = -ballRadius - 4 + getPaddlePosition().z;
	}
		
	if (within(pos)) {
		if (onTable(pos)) {
			ballVel = bounce(ballVelocity(time), table_normal, table_bounce);
			position = pos;
			time = 0;
			position.y = ballRadius;
		}
		if (onNet(pos)) {
			ballVel = bounce(ballVelocity(time), net_normal, net_bounce);
			position = pos;
			time = 0;
			if (pos.z > 0){
				position.z = 5 + ballRadius;
				textToSend = "Your net foul";
			}
			else{
				position.z = -5 -ballRadius;
				textToSend = "Their net foul";
			}
		}
	} 
	if (below(pos)){
		textToSend = "Out of bounds";
		reset();
	}
	if (points > 2) {
		textToSend = "You win!";
	}
}

/*
 * Reflect the ball along a plane using the formula, b * (-2 * (V dot N) * N + V) where:
 *		b - Bounce, 0.0 means no bounce and 1.0 means no loss of speed. The net 
 *				will have a smaller b value compared to the table
 *		V - Ball velocity
 *		N - Vector normal to the table / net
 */

void App::reset() {
	position = Vector3(0, 60, -130);
	ballVel = Vector3(0, 0, 52);
	time = 0;
	//reset points
	if (points > 2) {
		points = 0;
		textToSend = "New game";
	}
}
Vector3 App::bounce(Vector3 V, Vector3 N, float b) {
	return b * (-2 * (V.dot(N) * N) + V);
}

bool App::within(Vector3 pos) {
	return ((pos.x >= -76.25) && (pos.x <= 76.25) && (pos.z <= 137) && (pos.z >= -137));
}

bool App::below(Vector3 pos) {
	return (pos.y < -20);
}

bool App::onTable(Vector3 pos) {
	return (pos.y <= ballRadius);
}
bool App::onNet(Vector3 pos) {
	return (pos.y >= ballRadius && pos.y <= 15.25 + ballRadius) && (pos.z >= -5 - ballRadius && pos.z <= 5 + ballRadius) 
		&& (pos.x >= -84 -ballRadius && pos.x <= 84 + ballRadius);
}

bool App::hitPaddle(Vector3 pos) {
	Vector3 paddlePos = getPaddlePosition();
	return (pos.x >= paddlePos.x - 8 - ballRadius && pos.x <= paddlePos.x + 8 + ballRadius) &&
		(pos.x >= prev_position.x - 8 - ballRadius && pos.x <= prev_position.x + 8 + ballRadius) &&
		(pos.z >= prev_position.z - 2.5 - ballRadius && pos.z <= prev_position.z + 2.5 + ballRadius) && 
		(pos.y >= ballRadius);
}

Vector3 App::ballVelocity(double time) {
	return Vector3(ballVel.x, ballVel.y - gravity * time, ballVel.z);
}

Vector3 App::ballPos(double time) {
	return position + ((0.5 * (ballVelocity(time) + ballVel)) * time);
}

void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface>> &surface3D) {
	rd->clear();

	rd->pushState();
	rd->setObjectToWorldMatrix(paddleFrame);
	Cylinder paddle(Vector3(0,0,-0.5), Vector3(0,0,0.5), 8.0);
	Cylinder handle(Vector3(0,-7.5,0), Vector3(0,-16,0), 1.5);
	Draw::cylinder(paddle, rd, Color3(0.5,0,0), Color4::clear());
	Draw::cylinder(handle, rd, Color3(0.3,0.4,0), Color4::clear());
	rd->popState();  

	rd->pushState();
	rd->setObjectToWorldMatrix(paddleFrame2);
	Cylinder paddle2(Vector3(ballPos(time).x,ballPos(time).y, -0.5 -137), Vector3(ballPos(time).x,ballPos(time).y,0.5-137), 8.0);
	Cylinder handle2(Vector3(ballPos(time).x,ballPos(time).y -7.5,-137), Vector3(ballPos(time).x,ballPos(time).y-16,-137), 1.5);
	Draw::cylinder(paddle2, rd, Color3(0.5,0,0), Color4::clear());
	Draw::cylinder(handle2, rd, Color3(0.3,0.4,0), Color4::clear());
	rd->popState();

	// Make Table
	Box table(Vector3(-76.25, -10, -137), Vector3(76.25, 0, 137));
	Box stand(Vector3(-40, -10, 120), Vector3(40, -76, -120));
	LineSegment line5 = LineSegment::fromTwoPoints(Point3(0, 0, 137), Point3(0, 0, -137));
	Draw::box(table, rd, Color3(0.0, 0.0, 0.35), Color4(1.0,1.0,1.0));

	Draw::box(stand, rd, Color3::gray(), Color4::clear());
	Draw::lineSegment(line5, rd, Color3::white(), 50);
	
	// Make Net
	for (int i = -36; i < 37; i++){
		LineSegment netGradientVertical = LineSegment::fromTwoPoints(Point3(i * 2.5, 13, 0), Point3(i * 2.5, 0, 0));
		Draw::lineSegment(netGradientVertical, rd, Color3::white());
	}
	for (int j = 0; j < 6; j++){
		LineSegment netGradientHoriztonal = LineSegment::fromTwoPoints(Point3(-89, j * 2.5, 0), Point3(89, j * 2.5, 0));
		Draw::lineSegment(netGradientHoriztonal, rd, Color3::white());
	}
	Box netTop(Vector3(-91.5, 13, 0.5), Vector3(91.5, 15.25, -0.5));
	Draw::box(netTop, rd, Color3::white());
	Box netLeft(Vector3(-91.5, 0, 0.5), Vector3(-89, 13, -0.5)); 
	Box netRight(Vector3(91.5, 0, 0.5), Vector3(89, 13, -0.5));
	Box bottomLeft(Vector3(-91.5, -2.5, 0.5), Vector3(-76.25, 0, -0.5));
	Box bottomRight(Vector3(91.25, -2.5, 0.5), Vector3(76.25, 0, -0.5));
	Draw::box(netLeft, rd, Color3::white());
	Draw::box(netRight, rd, Color3::white());
	Draw::box(bottomLeft, rd, Color3::white());
	Draw::box(bottomRight, rd, Color3::white());
		
	// Make Ball
	rd->pushState();
	rd->setObjectToWorldMatrix(ballFrame);
	Sphere ball(ballPos(time), ballRadius);
	Draw::sphere(ball, rd, Color3::white(), Color4::clear());
	rd->popState();

	//Make Shadows
	//Ball
	rd->pushState();
	rd->setObjectToWorldMatrix(ballShadow);
	if (within(ballPos(time))){
		Cylinder shadow(Vector3(ballPos(time).x,.01,ballPos(time).z), Vector3(ballPos(time).x,0,ballPos(time).z), ballRadius);
		Draw::cylinder(shadow, rd, Color3::black(), Color4::clear());
	}
	rd->popState(); 
	// Paddle
	rd ->pushState();
		rd->setObjectToWorldMatrix(paddleShadow);
	if ((getPaddlePosition().x > -76.25 + 8) && (getPaddlePosition().x < 76.25 -8) &&
	(getPaddlePosition().z < 137)){
		Cylinder handleShadow(Vector3(getPaddlePosition().x,.01,getPaddlePosition().z), Vector3(getPaddlePosition().x,0,getPaddlePosition().z), 1.5);
		Box paddleShadow(Vector3(getPaddlePosition().x + 8,.01,getPaddlePosition().z + .5), Vector3(getPaddlePosition().x - 8,0,getPaddlePosition().z - .5));
		Draw::cylinder(handleShadow, rd, Color3::black(), Color4::clear());
		Draw::box(paddleShadow, rd, Color3::black(), Color4::clear()); 
	}
	rd->popState(); 
	// Call to make the GApp show the output of debugDraw
	drawDebugShapes();
}