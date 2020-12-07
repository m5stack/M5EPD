#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.EPD.Clear(true);
    //overlay bg on full screen
    canvas.createCanvas(540, 960);
    canvas.setTextSize(5);
    //jpg must be in 90degree (full screen)
    canvas.drawJpgFile(SD, "/u11.jpg");
    //load the scene starting at x0 y0 position
    canvas.pushCanvas(0,0,UPDATE_MODE_GC16);
    //after loading we kill canvas and rotate to overlay the text on the bg 
    canvas.deleteCanvas();
    M5.EPD.SetRotation(0);
    //set the new size of the scene-element (canvas)
    canvas.createCanvas(610, 140);
    canvas.setTextSize(3);
    canvas.drawString("The Screen! HP: 100% | DMG: 100 | Speed: 3 |", 0, 0);
    canvas.pushCanvas(200,400,UPDATE_MODE_GL16);
    //kill canvas overlay main image 
    canvas.deleteCanvas();
    M5.EPD.SetRotation(90);
    //create a draw area that will overwrite the screen 
    canvas.createCanvas(361,662);
    //pull image from sd card
    canvas.drawJpgFile(SD, "/mainwindow1.jpg");
    //create it on position with selected update mode  
    canvas.pushCanvas(150,204,UPDATE_MODE_GC16);


}

void loop()
{

}
