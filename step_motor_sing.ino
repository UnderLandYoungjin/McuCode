int en = 2;
int step = 3;
int dir = 4;

void setup() {
  // put your setup code here, to run once:
  pinMode(en, OUTPUT);
  pinMode(step, OUTPUT);
  pinMode(dir, OUTPUT);

  digitalWrite(en, LOW);
  digitalWrite(step, LOW);
  digitalWrite(dir, LOW);

  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i <5000; i++) //도
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(230);
    digitalWrite(step,LOW);
    delayMicroseconds(230);
  }
  delay(1000);
  for(int i = 0; i <5000; i++) //레
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(205);
    digitalWrite(step,LOW);
    delayMicroseconds(205);
  }
  delay(1000);
  for(int i = 0; i <5000; i++) //미
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(182);
    digitalWrite(step,LOW);
    delayMicroseconds(182);
  }
  delay(1000);
  for(int i = 0; i <5000; i++) //파
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(172);
    digitalWrite(step,LOW);
    delayMicroseconds(172);
  }
  delay(1000);
  for(int i = 0; i <5000; i++) //솔
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(152);
    digitalWrite(step,LOW);
    delayMicroseconds(152);
  }
  delay(1000);

   for(int i = 0; i <5000; i++) //라
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(135);
    digitalWrite(step,LOW);
    delayMicroseconds(135);
  }
  delay(1000);
  for(int i = 0; i <5000; i++) //시
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(120);
    digitalWrite(step,LOW);
    delayMicroseconds(120);
  }
  delay(1000);
  for(int i = 0; i <5000; i++) //도
  {
    digitalWrite(step,HIGH);
    delayMicroseconds(114);
    digitalWrite(step,LOW);
    delayMicroseconds(114);
  }
  delay(1000);



  
}
