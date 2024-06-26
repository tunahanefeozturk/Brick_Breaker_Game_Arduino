#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define ekranEn 128
#define ekranBoy 64
#define ballR 2
#define paletEn 30
#define paletBoy 2
#define brickEn 15
#define brickBoy 6
#define can1 11
#define can2 12
#define can3 13
#define photoPin 10
#define upButton 1
#define enterButton 2
#define downButton 0
#define potPin  A0
#define a 3
#define b 4
#define c 5
#define d 6
#define e 7
#define f 8
#define g 9

Adafruit_SSD1306 ekran(ekranEn, ekranBoy, &Wire, -1);

int boncukX, boncukY;
float speedX = 1;
float speedY = -1;
int16_t paletX;
bool runGame;
int skor = 0;
int pot;
int lastPot;
bool enterPressed = false;
bool upPressed = false;
bool downPressed = false;
bool lastEnterPressed = false;
bool Start = false;
bool Quit = false;
int can = 3;
bool Map[8][10];
int lvl = 1;
int lastLvl = 0;
int jokerX, jokerY;
bool jokerAct = false;

void setup() {
  Serial.begin(9600);

  pinMode(enterButton, INPUT_PULLUP);
  pinMode(photoPin, INPUT);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);



  if (!ekran.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  ekran.clearDisplay();
  ekran.setTextSize(1);
  ekran.setTextColor(SSD1306_WHITE);

  boncukX = ekranEn / 2;
  boncukY = ekranBoy - 10;
  speedX = 1;
  speedY = -1;
  paletX = (ekranEn - paletEn) / 2;
  pinMode(can1, OUTPUT);
  pinMode(can2, OUTPUT);
  pinMode(can3, OUTPUT);
  runGame = false;
}

void loop() {

  ekran.clearDisplay();


  if (digitalRead(photoPin) == HIGH) {
    ekran.invertDisplay(true);
  } else {
    ekran.invertDisplay(false);
  }
  if (digitalRead(upButton) == LOW) {
    upPressed = true;
    downPressed=false;
  }
  else if (digitalRead(downButton) == LOW) {
    downPressed = true;
    upPressed=false;
  }

  enterPressed = digitalRead(enterButton);

  if (enterPressed == LOW) {
    if (upPressed) {
      Start = true;
    }
    if (downPressed) {
      Quit = true;

    }
  }


  lastEnterPressed = enterPressed;

  pot = analogRead(potPin);

  if (pot != lastPot && !runGame && Start) {
    runGame = true;
  }
  lastPot = pot;



  if (runGame) {
    ballMove();
    collKont();
    paletX = map(pot, 0, 1023, 0, ekranEn - paletEn);

  } 

  if (lvl != lastLvl) {
    setLvl();

  }

  if (!Start) {
    showStart();
  }
  if (Quit && !Start) {
    showQuit();
    runGame = false;
  }
  if (Start ) {
    showGame();
  }
  if (can <= 0 || lvl > 5) {
    showEnd();
  }
  if (jokerAct && runGame) {
    showJoker();
  }

  ekran.display();
  heal();
  showScoreSevSeg();

  if (can <= 0 || lvl > 5 ) {
    delay(5000);
    lvl = 1;
    setLvl();
    can = 3;
    skor = 0;
    speedX = 1;
    speedY = -1;
    Start = false;
    Quit = false;
    upPressed=false;
    downPressed=false;
    runGame = false;
  }
  if (lvl != lastLvl) {
    delay(500);
  }
  delay(20);
}

void showGame() {
  showBrick();
  ekran.fillCircle(boncukX, boncukY, ballR, SSD1306_WHITE);

  ekran.fillRect(paletX, ekranBoy - paletBoy, paletEn, paletBoy, SSD1306_WHITE);
}

void showBrick() {
  ekran.clearDisplay();
  bool isempty = true;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (Map[i][j]) {
        ekran.fillRect(i * (brickEn + 1), j * (brickBoy + 1), brickEn, brickBoy, SSD1306_WHITE);
        isempty = false;
      }
    }
  }
  if (isempty) {
    lvl++;
    if (speedX < 0) {
      speedX = -speedX;
    }
    if (speedY < 0) {
      speedY = -speedY;
    }
    speedX += speedX / 5;
    speedY += speedY / 5;
    speedY = -speedY;
    btwLvl();
    can++;
    reset();
  }
}

void showStart() {
  ekran.clearDisplay();
  ekran.setCursor(50, 20);
  ekran.print("START");
  ekran.setCursor(50, 32);
  ekran.print("QUIT");

}
void btwLvl() {
  ekran.clearDisplay();
  ekran.setCursor(50, 30);
  ekran.print("LEVEL ");
  ekran.print(lvl);
  for (int i = 0; i < 10000; i++);

}

void showQuit() {
  ekran.clearDisplay();
  ekran.setCursor(40, 10);
  ekran.println("Oyunumuza");
  ekran.setCursor(30, 25);
  ekran.println("gosterdiginiz");
  ekran.setCursor(40, 40);
  ekran.println("ilgi icin");
  ekran.setCursor(35, 55);
  ekran.println("tesekkurler");

}

void showEnd() {
  ekran.clearDisplay();
  ekran.setCursor(35, 20);
  ekran.print("GAME OVER");
  ekran.setCursor(40, 40);
  ekran.print("SKOR:");
  ekran.print(skor);


}

void showJoker() {
  if (jokerY < ekranBoy - paletBoy) {
    ekran.setCursor(jokerX, jokerY);
    ekran.print("*");
    jokerY +=2;
  }
  else if (jokerX >= paletX && jokerX <= paletX + paletEn
           && jokerY + 15 >= ekranBoy - paletBoy) {
    if (can < 3) {
      can++;
    }
    jokerX = 0;
    jokerY = 0;
    jokerAct = false;

  } else {
    jokerX = 0;
    jokerY = 0;
    jokerAct = false;
  }
}

void ballMove() {
  boncukX += speedX;
  boncukY += speedY;
}

void collKont() {
  if (boncukX + ballR >= ekranEn || boncukX <= 0) {
    speedX *= -1;
  }
  if (boncukY <= 0) {
    speedY *= -1;
  }

  if (boncukY + ballR >= ekranBoy - paletBoy) {
    if (boncukX >= paletX && boncukX <= paletX + paletEn) {
      speedY *= -1;
    } else {
      runGame = false;
      reset();
    }
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 10; j++) {
      if (Map[i][j]) {
        int brickX = i * (brickEn + 1);
        int brickY = j * (brickBoy + 1);

        if (boncukX + ballR > brickX &&
            boncukX - ballR < brickX + brickEn &&
            boncukY + ballR > brickY &&
            boncukY - ballR < brickY + brickBoy) {
          Map[i][j] = false;

          if (boncukX > brickX + brickEn || boncukX < brickX ) {
            speedX = -speedX;
          }
          else if (boncukY < brickY || boncukY > brickY + brickBoy ) {
            speedY = -speedY  ;
          }
          skor++;

          int r = random(10);
          if (r == 0) {
            jokerX = brickX + (brickEn / 2);
            jokerY = brickY + (brickBoy / 2);
            jokerAct = true;
          }

        }
      }
    }
  }


}



void setLvl() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 10; j++) {
      Map[i][j] = false;
    }
  }
  switch (lvl) {
    case 1:
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 3; j++) {
          Map[i][j] = true;
        }
      }
      
      break;
    case 2:
      int i, j;
      for ( i = 0; i < 4; i++) {
        for ( j = 5 - i - 1; j < 5 ; j++) {
          Map[i][j] = true;
          Map[8 - i - 1][j] = true;
        }
      }
      
      break;
    case 3:
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 5; j++) {
          if ((i % 2 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 2 == 0)) {
            Map[i][j] = true;
          }
        }
      }
      
      break;
    case 4:
      for (int i = 0; i < 8; i++) {
        for (j = 0; j < 5; j++) {
          if (i == 2 || i == 5 || j == 0 || j == 4) {
            Map[i][j] = true;
          }
        }
      }
      
      break;
    case 5:
      for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 5 ; j++) {
          if (((j != 0 && j != 4) && (i == 2 || i == 5)) 
          || (!(j != 0 && j != 4) && !(i == 2 || i == 5))) {
            Map[i][j] = true;
          }
        }
      }
      
      break;
  }
  lastLvl = lvl;
}

void reset() {
  boncukX = paletX + paletEn / 2;
  boncukY = ekranBoy - 10;
  speedY = -speedY;
  lastPot = pot;
  can = can - 1;

}


void heal() {

  switch (can) {
    case 0:
      digitalWrite(can1, LOW);
      digitalWrite(can2, LOW);
      digitalWrite(can3, LOW);

      break;
    case 1:
      digitalWrite(can1, HIGH);
      digitalWrite(can2, LOW);
      digitalWrite(can3, LOW);
      break;
    case 2:
      digitalWrite(can1, HIGH);
      digitalWrite(can2, HIGH);
      digitalWrite(can3, LOW);
      break;
    case 3:
      digitalWrite(can1, HIGH);
      digitalWrite(can2, HIGH);
      digitalWrite(can3, HIGH);
      break;

  }
}
void showScoreSevSeg() {

  int mod = skor % 10;
  switch (mod) {
    case 0:
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, HIGH);

      break;
    case 1:
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    case 2:
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, HIGH);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, HIGH);
      digitalWrite(g, LOW);
      break;
    case 3:
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, LOW);
      break;
    case 4:
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
      break;
    case 5:
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
      break;
    case 6:
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
      break;
    case 7:
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, HIGH);
      digitalWrite(e, HIGH);
      digitalWrite(f, HIGH);
      digitalWrite(g, HIGH);
      break;
    case 8:
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, LOW);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
      break;
    case 9:
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
      digitalWrite(c, LOW);
      digitalWrite(d, LOW);
      digitalWrite(e, HIGH);
      digitalWrite(f, LOW);
      digitalWrite(g, LOW);
      break;
  }

}

