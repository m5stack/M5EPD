#include <M5EPD.h>

M5EPD_Canvas canvas(&M5.EPD);

int point[2][2];

void setup()
{
    M5.begin();
    M5.EPD.SetRotation(90);
    M5.TP.SetRotation(90);
    M5.EPD.Clear(true);
    canvas.createCanvas(540, 960);
    canvas.setTextSize(5);
    canvas.drawString("Touch The Screen!", 20, 400);
    canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
}

void loop()
{

    if(M5.TP.avaliable()){
        if(!M5.TP.isFingerUp()){
            M5.TP.update();
            canvas.fillCanvas(0);
            bool is_update = false;
            for(int i=0;i<2; i++){
                tp_finger_t FingerItem = M5.TP.readFinger(i);
                if((point[i][0]!=FingerItem.x)||(point[i][1]!=FingerItem.y)){
                    is_update = true;
                    point[i][0] = FingerItem.x;
                    point[i][1] = FingerItem.y;
                    canvas.fillRect(FingerItem.x-50, FingerItem.y-50, 100, 100, 15);
                    Serial.printf("Finger ID:%d-->X: %d*C  Y: %d  Size: %d\r\n", FingerItem.id, FingerItem.x, FingerItem.y , FingerItem.size);
                }
            }
            if(is_update)
            {
                canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
            }
        }
    }
}