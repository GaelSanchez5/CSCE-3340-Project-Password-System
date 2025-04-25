#include "stm32c0xx.h"


const char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};


const char str0[13] = "Set Password";
const char str1[15] = "Enter Password";
const char str2[8] = "Correct";
const char str3[10] = "Incorrect";
const char str4[16] = "Reset Password?";
const char str5[13] = "1-Yes 2-No _";


char password[5];
char inputPassword[5];


void delay_ms(uint32_t ms){
  for(uint32_t i = 0; i < ms * 2000; i++){
    __asm__("nop");
  }
}


void writeCommand(uint8_t command){
  GPIOA->ODR &= ~(1 << 8);




  GPIOA->ODR &= ~(0XFF << 0);
  GPIOA->ODR |= (command << 0);




  GPIOA->ODR |= (1 << 9);
  delay_ms(1);
  GPIOA->ODR &= ~(1 << 9);
  delay_ms(1);
}




void writeData(char data){
  GPIOA->ODR |= (1 << 8);




  GPIOA->ODR &= ~(0XFF << 0);
  GPIOA->ODR |= (data << 0);




  GPIOA->ODR |= (1 << 9);
  delay_ms(1);
  GPIOA->ODR &= ~(1 << 9);
  delay_ms(1);
}




void lcdStart(){
  RCC->IOPENR |= (1 << 0);




  for(uint32_t i = 0; i < 10; i++ ){
    GPIOA->MODER &= ~(3 << (2 * i));
    GPIOA->MODER |= (1 << (2 * i));
  }


 writeCommand(0x38);
 writeCommand(0x06);
 writeCommand(0x0E);
}






void keypadStart(){
  RCC->IOPENR |= (1 << 1);
 
  for(uint8_t i = 0; i < 4; i++){
  GPIOB->MODER &= ~(3 << (2 * i));
  GPIOB->MODER |= (1 << (2 * i));
  }


  for(uint8_t i = 4; i < 8; i++){
    GPIOB->MODER &= ~(3 << (2 * i));
    GPIOB->PUPDR &= ~(3 << (2 * i));
    GPIOB->PUPDR |= (2 << (2 * i));
  }
}


char getKey(){
  for(uint8_t i = 0; i < 4; i++){
    GPIOB->ODR &= ~(0XF << 0);
    GPIOB->ODR |= (1 << i);
    delay_ms(1);


    for(uint8_t j = 0; j < 4; j++){
      if(GPIOB->IDR & (1 << (j + 4))){
        while(GPIOB->IDR & (1 << (j + 4)));
        GPIOB->ODR &= ~(0XF << 0);
      return keys[i][j];
      }
    }
  }
  return 0;
}


void setPassword(){
   char key;
   uint8_t index = 0;
   uint8_t column = 0xC0;


   writeCommand(0x01);
   for(uint8_t i = 0; i < 12; i++){
    writeData(str0[i]);
   }
   writeCommand(0xC0);
   for(uint8_t i = 0; i < 4; i++){
    writeData('_');
   }
   writeCommand(0xC0);


  while(1){
   key = getKey();


  if(key != 0){
    if(key == '#'){
      if(index == 4){
        return;
      }


    }


    else if(key == '*'){
      if(index != 0){
        column--;
        index--;
        writeCommand(column);
        writeData('_');
        writeCommand(column);
      }




    }


    else{
     if(index < 4) {
    password[index] = key;
    writeData('*');
  index++;
  column++;
     }
    }


   }
   }


}


void checkPassword(){
   char key;
   uint8_t index = 0;
   uint8_t column = 0xC0;


   writeCommand(0x01);


   for(uint8_t i = 0; i < 14; i++){
    writeData(str1[i]);
   }
   writeCommand(0xC0);
   for(uint8_t i = 0; i < 4; i++){
    writeData('_');
   }
   writeCommand(0xC0);


  while(1){
   key = getKey();


  if(key != 0){
    if(key == '#'){
      if(index == 4){
        return;
      }


    }


    else if(key == '*'){
      if(index != 0){
        column--;
        index--;
        writeCommand(column);
        writeData('_');
        writeCommand(column);
      }




    }


    else{
     if(index < 4) {
    inputPassword[index] = key;
    writeData('*');
  index++;
  column++;
     }
    }


   }
   }


}


bool verifyPassword(){
  for(uint8_t i = 0; i < 4; i++){
    if(password[i] != inputPassword[i]){
      return 0;
    }
  }
  return 1;
}


void lock(){
  writeCommand(0x01);
    writeData('1');
    writeData('0');
    delay_ms(500);
    for(char i = '9'; i != '0'; i--){
      writeCommand(0x01);
      writeData(i);
      delay_ms(500);
    }
}


bool confirmReset(){
  writeCommand(0x01);
  for(uint8_t i = 0; i < 15; i++){
    writeData(str4[i]);
  }


  writeCommand(0xC0);
  for(uint8_t i = 0; i < 12; i++ ){
  writeData(str5[i]);
  }
  writeCommand(0xCB);
   
   while(1){
   char key = getKey();


   if(key != 0){
    if(key == '1'){
      return 1;
    }
    else if(key == '2'){
      return 0;
    }
   }


   }


}


int main(void){
  bool verified = 0;
  bool repeat = 1;
  uint8_t attempts = 0;


   lcdStart();
   keypadStart();


 while(1){
   if(repeat == 1)
   setPassword();
   do{
   checkPassword();


   if(verifyPassword()){
    writeCommand(0x01);


   for(uint8_t i = 0; i < 7; i++){
    writeData(str2[i]);
   }
   verified = 1;
   attempts = 0;
   delay_ms(1000);
   }
   else{
    writeCommand(0x01);


   for(uint8_t i = 0; i < 9; i++){
    writeData(str3[i]);
   }
   attempts++;
   delay_ms(1000);


   if(attempts >= 3){
   lock();
    attempts = 0;
   }
   verified = 0;
   }
   }while(!verified);


  repeat = confirmReset();
 }
}
