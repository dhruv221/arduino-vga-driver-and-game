
#define nop __asm__("nop \n")


#define SCREEN_W 40
#define SCREEN_H 30

char PXL_DATA [SCREEN_H][SCREEN_W]; 
int  lineCounter;
char skipLine;
char PXL_OUT;
char dy;
char enemyX;
char enemyY;

float score;

ISR(TIMER1_OVF_vect)
  {lineCounter = 0;}


ISR(TIMER2_OVF_vect)
  {
    sei();
    __asm__("sleep \n");
  }



ISR(TIMER2_COMPB_vect)
{
  register char  columnCounter;
  register char* rowPtr;

  switch (PXL_OUT){
    case 1:
    rowPtr = &(PXL_DATA[(lineCounter - 35)>>4][0]);
    columnCounter = SCREEN_W;

    while(columnCounter--){
      PORTD = (*(rowPtr++)<<PD2);
      nop;
    }
    nop;
    nop;
    nop;
    PORTD = 0;
    break;
  }

  lineCounter++;
  skipLine = !skipLine;

  if ( skipLine && (lineCounter >= 35) && (lineCounter < 515) ){
    PXL_OUT = 1;
  }
  else{
    PXL_OUT = 0;
  }
}


void setup() {
  cli();                   //DISABLE INTERRUPTS
  PORTD = 0;
  pinMode(10,OUTPUT);    
  pinMode(3,OUTPUT);
  pinMode(2,OUTPUT);
  
  pinMode(A0,INPUT);
  pinMode(12, INPUT);
 
  TCCR1A = 0b00100011;     //PIN D10 OUTPUT, FAST PWM MODE 
  TCCR1B = 0b00011101;     //FAST PWM, PRESCALER OF 1024
  TIMSK1 = 0b00000001;     //OVERFLOW INTERRUPT ENABLE
  TIFR1  = 0b00000001;     //CLEAR OVERFLOW FLAG
  //TCNT1  = 0;            //rst t1
  TCCR2A = 0b00100011;     //PIN D3 OUTPUT, FAST PWM MODE
  TCCR2B = 0b00001010;     //FAST PWM, PRESCALER OF 8
  TIMSK2 = 0b00000101;     //OVERFLOW INTERRUPT ENABLE
  TIFR2  = 0b00000101;     //CLEAR OVERFLOW FLAG
  //TCNT2  = 0;            //rst t2 
  OCR1A  = 259;            //SET FREQ OF T1
  OCR1B  = 0;              //VSYNC PULSE DURATION
  OCR2A  = 63;             //SET FREQ OF T2
  OCR2B  = 7;              //HSYNC PULSE DURATUIN
  SMCR = bit(SE);
  memset(PXL_DATA, 0, sizeof(PXL_DATA));
  lineCounter = 0;
  enemyX =random(1,36);
  enemyY =random(3,25);
  score=0;
  sei();
}

void gameCode(){  
}

void loop() {
  char col = (analogRead(A0)/29);
  bool shoot = digitalRead(12);
  bool bulletFired;
  char bulletX;
  char bulletY;

  
  
  if (shoot == 1){
    bulletFired = 1;
    dy = 0.0;  
    bulletX = col;
  }
  if ((bulletFired==1)&&(dy<=27)){
    bulletY = 27-dy;
    dy = dy + 1;
  } 
  if ( (bulletY<=0)&&(dy>=27) ){
    bulletFired = 0;
    bulletY=31;
  }
  if ( (bulletX==(enemyX-1))&&(bulletY==enemyY) ){
    enemyX=random(1,36);
    enemyY=random(3,25);
    bulletFired = 0;
    dy = 27;
    score = score + 4;
    if (score>=37){
      score=0;
    }
  }
  

  char r, c;

  
  for (r = 0; r < SCREEN_H; r++) {
    for (c = 0; c < SCREEN_W; c++) {
      if ((r >= 28) &&
        (r < 28 + 1) &&
        (c >= col) &&
        (c < col + 3)) {
        PXL_DATA[r][c] = 0;
      }
      else if ((r >= 27) &&
        (r < 27 + 1) &&
        (c >= col+1) &&
        (c < col + 2)) {
        PXL_DATA[r][c] = 0;
      }
      else if ((bulletFired==1)&&(r >= bulletY) &&
        (r < bulletY + 1) &&
        (c >= bulletX+1) &&
        (c < bulletX + 2)) {
        PXL_DATA[r][c] = 0;
      }
      else if ((r >= enemyY) &&
        (r < enemyY + 1) &&
        (c >= enemyX) &&
        (c < enemyX + 1)) {
        PXL_DATA[r][c] = 0;
      }
      else if ((r >= 0) &&
        (r < 0 + 1) &&
        (c >= 0) &&
        (c < 0 + score)) {
        PXL_DATA[r][c] = 0;
      }
      else {
        PXL_DATA[r][c] = 1;
      }      
    }
  } 
}
