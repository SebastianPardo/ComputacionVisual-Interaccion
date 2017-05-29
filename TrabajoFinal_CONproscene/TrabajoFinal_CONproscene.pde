import processing.serial.*;

import remixlab.proscene.*;
import remixlab.bias.*;
import remixlab.bias.event.*;
import remixlab.dandelion.geom.*;
import remixlab.dandelion.core.*;

import org.gamecontrolplus.*;
import net.java.games.input.*;

Serial myPort;
PImage bg;
PImage texmap;

int sDetail = 35;  // Sphere detail setting
float rotationX = 0;
float rotationY = 0;
float velocityX = 0;
float velocityY = 0;
float globeRadius = 400;
float pushBack = 0;

float[] cx, cz, sphereX, sphereY, sphereZ;
float sinLUT[];
float cosLUT[];
float SINCOS_PRECISION = 0.5;
int SINCOS_LENGTH = int(360.0 / SINCOS_PRECISION);

Scene scene;
static int SN_ID;
InteractiveFrame iFrame;
HIDAgent hidAgent;

float sliderXpos; // Positions
float sliderYpos;
float sliderZpos;

public class HIDAgent extends Agent {
  // array of sensitivities that will multiply the sliders input
  // found pretty much as trial an error
  float [] sens = {10, 10, 10};
  
  public HIDAgent(Scene scn) {
    super(scn.inputHandler());
    // SN_ID will be assigned an unique id with 6 DOF's. The id may be
    // used to bind (frame) actions to the gesture, pretty much in
    // the same way as it's done with the LEFT and RIGHT mouse gestures.
    SN_ID = MotionShortcut.registerID(2, "SN_SENSOR");
    addGrabber(scene.eyeFrame());
    setDefaultGrabber(scene.eyeFrame());
  }
  
  // we need to override the agent sensitivities method for the agent
  // to apply them to the input data gathered from the sliders
  @Override
  public float[] sensitivities(MotionEvent event) {
    if (event instanceof DOF2Event)
      return sens;
    else
      return super.sensitivities(event);
  }
  
  // polling is done by overriding the feed agent method
  // note that we pass the id of the gesture
  @Override
  public DOF2Event feed() {
    println(frameCount);
    print(sliderXpos,"-",sliderYpos, "-" ,sliderZpos,"\n");
    return new DOF2Event(sliderXpos, sliderYpos, BogusEvent.NO_MODIFIER_MASK, SN_ID);
  }
}

void setup() {
  size(800, 600, P3D);
  conection("COM3");
  texmap = loadImage("world32k.jpg");
  scene = new Scene(this);
  
  scene.setGridVisualHint(false);
  scene.setAxesVisualHint(false);  
  scene.setRadius(260);
  scene.showAll();

  hidAgent = new HIDAgent(scene);
  
  // the iFrame is added to all scene agents (that's why we previously instantiated the hidAgent)
  // Thanks to the Processing Foundation for providing the rocket shape
  //iFrame = new InteractiveFrame(scene, loadShape("rocket.obj"));
  //iFrame.translate(new Vec(275, 180, 0));
  //iFrame.scale(0.3);
  
  // we bound some frame DOF6 actions to the gesture on both frames
  scene.eyeFrame().setMotionBinding(SN_ID, "translate");
  //iFrame.setMotionBinding(SN_ID, "translateRotateXYZ");
  // and the custom behavior to the right mouse button
  //iFrame.setMotionBinding(RIGHT, "customBehavior");

  smooth();
}

//void customBehavior(InteractiveFrame frame, MotionEvent event) {
  //frame.screenRotate(event);
//}

void draw() {    
  background(0);
  box(100,100,100);
  scene.drawFrames();
}

void conection(String portName){
  myPort = new Serial(this, portName, 9600);
  myPort.buffer(3); //event when two characters received
  myPort.clear();
}

void serialEvent(Serial myPort) { 
  sliderXpos = myPort.read(); 
  sliderYpos = myPort.read();
  sliderZpos = myPort.read();
  print(sliderXpos,"-",sliderYpos, "-" ,sliderZpos,"\n");
  //print(sliderYpos);
  //print(sliderZpos);
} 