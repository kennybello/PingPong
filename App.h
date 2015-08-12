/**
    CSci-4611 Spring '14 Assignment #2 Pong 3D
**/

#ifndef App_h
#define App_h		

#include <G3D/G3DAll.h>

class App : public GApp {
public:
    
  App(const GApp::Settings& settings = GApp::Settings());

  virtual void onInit();

  virtual void onUserInput(UserInput *uinput); 
  
  virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt);
  
  virtual void onGraphics3D(RenderDevice* rd, Array< shared_ptr<Surface> >& surface);


  // Use these functions to access the current state of the paddle!
  Vector3 getPaddlePosition() { return paddleFrame.translation; }
  Vector3 getPaddleNormal() { return Vector3(0,0,-1); }
  Vector3 getPaddleVelocity() { return paddleVel; }

  shared_ptr<GuiWindow> window;
  GuiTextBox* textBox;
  String textToSend;

protected:

  // This CoordinateFrame stores position and rotation data for the paddle.
  CoordinateFrame paddleFrame;
  CoordinateFrame paddleFrame2;
  CoordinateFrame ballFrame;
  CoordinateFrame ballShadow, paddleShadow;
  
  // This vector stores the paddle's current velocity.
  Vector3 paddleVel;

private:
    virtual Vector3 ballPos(double time);
	virtual bool onTable(Vector3 pos);
	virtual bool onNet(Vector3 pos);
	virtual bool within(Vector3 pos);
	virtual bool below(Vector3 pos);
	virtual void reset();
	virtual bool hitPaddle(Vector3 pos);
	virtual Vector3 ballVelocity(double time);
	Vector3 position, ballVel;
	Vector3 prev_position;
	virtual Vector3 bounce(Vector3 V, Vector3 N, float b);
	double net_bounce, table_bounce;
	double ballRadius;
    
	Vector3 table_normal;
	Vector3 net_normal;
	double gravity;
	double time;
	double points;
};

#endif
