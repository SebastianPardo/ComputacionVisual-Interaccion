import processing.serial.*;

import remixlab.proscene.*;
import remixlab.bias.*;
import remixlab.bias.event.*;
import remixlab.dandelion.geom.*;
import remixlab.dandelion.core.*;

import org.gamecontrolplus.*;
import net.java.games.input.*;

Serial myPort;
Scene scene;
static int SN_ID;
InteractiveFrame iFrame;
HIDAgent hidAgent;

float sliderXpos; // Positions
float sliderYpos;
float sliderZpos;
boolean flag = false;

float dataX = 255/2;
float dataY = 255/2;
float dataZ = 0;


public class HIDAgent extends Agent {
  float [] sens = {1, 1, 1};
  
  public HIDAgent(Scene scn) {
    super(scn.inputHandler());
    SN_ID = MotionShortcut.registerID(2, "SN_SENSOR");
    addGrabber(scene.eyeFrame());
    setDefaultGrabber(scene.eyeFrame());
  }
  @Override
  public float[] sensitivities(MotionEvent event) {
    if (event instanceof DOF2Event)
      return sens;
    else
      return super.sensitivities(event);
  }
  @Override
  public DOF2Event feed() {
    flag = true;
    return new DOF2Event(sliderXpos, sliderYpos ,BogusEvent.NO_MODIFIER_MASK, SN_ID);  
  }
}

void setup() {
  size(1000, 600, P3D);
  conection("COM3");
  scene = new Scene(this);
  hidAgent = new HIDAgent(scene);
  scene.eyeFrame().setMotionBinding(SN_ID, "translate");
  smooth();
}
void draw() {    
  background(0);
  rotateX(2.0);
  rotateY(2.0);
  box(50,50,50);
  scene.drawFrames();
}

void conection(String portName){
  myPort = new Serial(this, portName, 9600);
  myPort.buffer(3); 
  myPort.clear();
}

float [] movement(float dataA, float dataB, float move){
  float result [] = {0,0}; 
  float slider=0;
  if(dataA == dataB || dataA - 1 == dataB || dataA + 1 == dataB){
      slider = 0;
    }else{
       if (dataA < dataB){
         dataB = dataB - 1;
         slider = -move;
       }
       if(dataA > dataB){
         dataB = dataB + 1;
         slider = move;
       }
    }
  result[0] = dataB;
  result[1] = slider;
  return result;
}

void serialEvent(Serial myPort) { 
  float data1 = myPort.read(); 
  float data2 = myPort.read();
  float data3 = myPort.read();
  
  float moveX = width/255;
  float moveY = height/255;
  
float result []={0,0}; 
   if (flag == true){
    //movimiento en X
    result = movement(data1, dataX, moveX);
    dataX = result[0];
    sliderXpos = result[1];
  
    //movimiento en Y
    result = movement(data2, dataY, moveY);
    dataY = result[0];
    sliderYpos = result[1];
   }
} 