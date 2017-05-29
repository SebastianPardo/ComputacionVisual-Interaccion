import processing.serial.*;

Serial myPort;  // Create object from Serial class
int data[] = {0, 0, 0};
int Color[] = {0, 0, 0};

void setup() 
{
  size(600, 600, P3D);
  conection("COM3");
}

void draw()
{
  background(0);
  fill(0,0,255);
  translate(width/2, height/2);
  //int getData [] = selectUtility();
  //rotateX(getData[0]);
  //rotateY(getData[1]);
  
  myColor(12,123,255);
  //selectUtility("zoom");
  selectUtility("light","translateY");
  //selectUtility("translateX","translateY", "light");
  rotateX(2);
  rotateY(2);
  box(100, 100, 100);
}

void conection(String portName){
  myPort = new Serial(this, portName, 9600);
  myPort.buffer(3); //event when two characters received
  myPort.clear();
}

int[] selectUtility(){
  return data;
}

void selectUtility(String utility){
  //print(utility);
  switch(utility){
    case "translateX":
      translate(data[0], height/2);
      break;
      
    case "translateY":
      translate(width/2, data[0]);
      break;
    
    case "light":
      translate(width/2, height/2);
      fill(Color[0], Color[1], Color[2], data[0]);
      
      break;
      
    case "zoom":
      translate(width/2, height/2, data[0]);
      break;
  }
}

void selectUtility(String utility1, String utility2){
  if(utility1 == utility2){
    print("tiene que ser dos parametros diferentes");
  }else{
    switch(utility2){
      case "translateX":
        selectUtility(utility1);
        translate(data[1], height/2);
        break;
        
      case "translateY":
        selectUtility(utility1);
        translate(width/2, data[1]);
        break;
      
      case "light":
        selectUtility(utility1);
        fill(Color[0], Color[1], Color[2], data[1]);
        break;
        
      case "zoom":
        selectUtility(utility1);
        translate(width/2, height/2, data[1]);
        break;
    }
  }
}

void selectUtility(String utility1, String utility2, String utility3){
  if(utility1 == utility2 || utility1 == utility3 || utility2 == utility3){
    print("tiene que ser dos parametros diferentes");
  }else{
    switch(utility3){
      case "translateX":
        selectUtility(utility1, utility2);
        translate(data[2], height/2);
        break;
        
      case "translateY":
        selectUtility(utility1, utility2);
        translate(width/2, data[2]);
        break;
      
      case "light":
        selectUtility(utility1, utility2);
        fill(Color[0], Color[1], Color[2], data[2]);
        break;
        
      case "zoom":
        selectUtility(utility1, utility2);
        translate(width/2, height/2, data[2]);
        break;
    }
  }
}

void myColor(int R, int G, int B){
  fill(R,G,B);
  Color[0] = Color[0] + R;
  Color[1] = Color[1] + G;
  Color[2] = Color[2] + B;
}

void serialEvent(Serial myPort) { 
  int val1 = int(myPort.read()); 
  int val2 = int(myPort.read());
  int val3 = int((myPort.read()-110)*5);
  int values [] = {val1, val2, val3};
  data = values;
} 