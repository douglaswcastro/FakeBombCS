#include <Wire.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

// Defina o endereço I2C do adaptador
#define I2C_ADDR 0x27  // Endereço I2C do adaptador. Pode variar dependendo do módulo.

// Defina o número de colunas e linhas da tela LCD
#define LCD_COLS 16
#define LCD_ROWS 2

// Inicialize o objeto LCD
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'a' },
  { '4', '5', '6', 'b' },
  { '7', '8', '9', 'c' },
  { '*', '0', '#', 'd' }
};

byte rowPins[ROWS] = {
  12, 13, A5, A4
};  //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  A3, A2, A1, A0
};  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char codeInput[4];
byte time[4];
boolean refresh = true;  //1 refresh one time...
char password[4];
int key = -1;
char lastKey;
char var;
boolean passwordEnable = false;

//Buttons for lcd shield
char BT_RIGHT = '4';
char BT_UP = 'a';
char BT_DOWN = 'b';
char BT_LEFT = '6';
char BT_SEL = 'd';  // Ok key
char BT_CANCEL = 'c';
char BT_DEFUSER = 'x';  // not implemented

//leds

const int REDLED = 11;
const int GREENLED = 10;
//const int BLUELED = 12;
//RELAYPIN
boolean relayEnable = false;
const int RELAYPIN = 9;
//IS VERY IMPORTANT THAT YOU TEST THIS TIME. BY DEFAULT IS IN 1 SEC. THAT IS NOT TOO MUCH. SO TEST IT!
const int RELAY_TIME = 5000;

//TIME INTS
int GAMEHOURS = 0;
int GAMEMINUTES = 45;
int BOMBMINUTES = 4;
int ACTIVATESECONDS = 5;

boolean endGame = false;

boolean sdStatus = false;  //search and destroy game enable used in config
boolean saStatus = false;  //same but SAbotaghe
boolean doStatus = false;  //for DEmolition
boolean start = true;
boolean defusing;
boolean cancelando;
// SOUND TONES
boolean soundEnable = true;
int tonepin = 8;  // Pin 13 for the sound
int alarmTone1 = 700;
int alarmTone2 = 2600;
int activeTone = 1330;
int errorTone = 100;

unsigned long iTime;
unsigned long timeCalcVar;
unsigned long redTime;
unsigned long greenTime;
unsigned long iZoneTime;  //initial time for zone
byte team = 0;            // 0 = neutral, 1 = green team, 2 = red team

void setup() {
  Wire.begin();
  Serial.begin(9600);
  // Inicialize a tela LCD
  lcd.init();
  // Ativar a retroiluminação da tela LCD (opcional)
  lcd.backlight();
  lcd.setCursor(3, 0);

  lcd.setCursor(0, 0);
  lcd.print("BLUECORE TECH");  // you can add your team name or someting cool
  lcd.setCursor(0, 1);
  lcd.print("AIRSOFT BOMB");  // you can add your team name or someting cool
  keypad.setHoldTime(50);
  keypad.setDebounceTime(50);
  keypad.addEventListener(keypadEvent);

  //PinModes
  pinMode(GREENLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(RELAYPIN, OUTPUT);
  // CONFIGURE THE BARS OF PROGRESS BAR
  byte bar1[8] = {
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
    B10000,
  };
  byte bar2[8] = {
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
  };
  byte bar3[8] = {
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
    B11100,
  };
  byte bar4[8] = {
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
    B11110,
  };
  byte bar5[8] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
  };
  byte up[8] = {
    B00000,
    B00100,
    B01110,
    B11111,
    B11111,
    B00000,
    B00000,
  };

  byte down[8] = {
    B00000,
    B00000,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
  };
  lcd.createChar(0, bar1);
  lcd.createChar(1, bar2);
  lcd.createChar(2, bar3);
  lcd.createChar(3, bar4);
  lcd.createChar(4, bar5);
  lcd.createChar(5, up);
  lcd.createChar(6, down);
}

void loop() {
  menuPrincipal();
}
void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case RELEASED:
      switch (key) {
        case 'd':
          defusing = false;
          break;
        case 'c':
          cancelando = false;
          break;
      }
      break;
    case HOLD:
      switch (key) {
        case 'd':
          defusing = true;
          break;
        case 'c':
          cancelando = true;
          break;
      }
      break;
  }
}

  char* menu1[]={"Search&Destroy","Sabotage","Domination", "Configuration"      };
  char* menu2[]={"Game Config","Sound Config", "Relay Test", "Auto Test"      };   
  char* GAME_TIME="Game Time:";
  char* BOMB_TIME="Bomb Time:";
  char* ZERO_MINUTES="00 minutes";
  char* ARM_TIME="Arm Time:";
  char* ZERO_SECS="00 seconds";
  char* ENABLE_SOUND="Enable Sound?";
  char* YES_OR_NOT="A : Yes B : No";
  char* ENABLE_RELAYPIN="Enable Relay?";
  char* ENABLE_CODE="Enable Code Arm?";
  char* GAME_TIME_TOP="GAME TIME";
  char* ARMING_BOMB = "ARMING BOMB";
  char* ENTER_CODE = "Enter Code";
  char* CODE_ERROR = "Code Error!";
  char* BOMB_ARMED = "BOMB ARMED";
  char* DETONATION_IN = "DETONATION IN";
  char* DISARMING = "DISARMING BOMB" ;
  char* DISARM = "DISARMING";
  char* GAME_OVER = " GAME OVER! ";
  char* DEFENDERS_WIN = " DEFENDERS WIN ";
  char* SABOTAGE_FAIL= "SABOTAGE FAIL!";


//##################MENUS###############################

void menuPrincipal() {  //MAIN MENU

  digitalWrite(GREENLED, LOW);
  digitalWrite(REDLED, LOW);

  //   if whe start a new game from another we need to restart propertly this variables
  saStatus = false;
  sdStatus = false;
  doStatus = false;
  //Draw menu
  cls();  //clear lcd and set cursor to 0,0
  int i = 0;
  // HERE YOU CAN ADD MORE ITEMS ON THE MAIN MENU
  lcd.print(menu1[i]);
  lcd.setCursor(15, 1);
  checkArrows(i, 2);
  while (1) {

    var = keypad.waitForKey();
    if (var == BT_UP && i > 0) {
      tone(tonepin, 2400, 30);
      i--;
      cls();
      lcd.print(menu1[i]);
      checkArrows(i, 2);
      delay(50);
    }
    if (var == BT_DOWN && i < 2) {
      tone(tonepin, 2400, 30);
      i++;
      cls();
      lcd.print(menu1[i]);
      checkArrows(i, 2);
      delay(50);
    }

    if (var == BT_SEL) {
      tone(tonepin, 2400, 30);
      cls();
      switch (i) {

        case 0:
          sdStatus = true;
          configQuickGame();
          startGameCount();
          search();
          break;
        case 1:
          saStatus = true;
          configQuickGame();
          startGameCount();
          sabotage();
          break;
        case 2:

          doStatus = true;
          configQuickGame();
          startGameCount();
          domination();
          break;
        case 3:
          config();
          break;
      }
    }
  }
}

void config() {
  //Draw menu
  lcd.clear();
  lcd.setCursor(0, 0);
  int i = 0;

  delay(500);
  lcd.print(menu2[i]);
  checkArrows(i, 3);

  while (1) {
    var = keypad.waitForKey();
    if (var == BT_UP && i > 0) {
      tone(tonepin, 2400, 30);
      i--;
      lcd.clear();
      lcd.print(menu2[i]);
      checkArrows(i, 3);
      delay(50);
    }
    if (var == BT_DOWN && i < 3) {
      tone(tonepin, 2400, 30);
      i++;
      lcd.clear();
      lcd.print(menu2[i]);
      checkArrows(i, 3);
      delay(50);
    }
    if (var == BT_CANCEL) {
      tone(tonepin, 2400, 30);
      menuPrincipal();
    }
    if (var == BT_SEL) {
      tone(tonepin, 2400, 30);
      lcd.clear();
      switch (i) {

        case 0:
          //gameConfigMenu();
          break;

        case 1:
          //soundConfigMenu();
          break;

        case 2:
          cls();
          lcd.print("RELÉPIN LIGADO!");
          digitalWrite(RELAYPIN, HIGH);  // liga o relé (HIGH é o nível de tensão)
          delay(4000);                   // aguarda 4 segundos
          cls();
          lcd.print("RELÉPIN DESLIGADO!");
          digitalWrite(RELAYPIN, LOW);
          delay(2000);
          config();
          break;
      }
    }
  }
}

void configQuickGame() {

  cls();
  //GAME TIME
  if (sdStatus || doStatus || saStatus) {
menu1:
    cls();
    lcd.print(GAME_TIME);
    delay(100);
    lcd.setCursor(0, 1);
    lcd.print("00:00 hh:mm");
    lcd.cursor();
    lcd.blink();
    lcd.setCursor(0, 1);
    byte var2 = 0;
    for (int i = 0; i < 4; i++) {
      while (1) {
        if (i == 2 && var2 == 0) {
          lcd.print(":");
          var2 = 1;
        }

        byte varu = getRealNumber();
        if (varu != 11) {

          time[i] = varu;
          Serial.print(varu);


          lcd.print(varu);
          tone(tonepin, 2400, 30);

          break;
        }
      }
    }
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(13, 1);
    lcd.print("ok?");
    //zona donde pasamos los items a
    //redibujar
    while (1) {
      var = keypad.waitForKey();
      if (var == 'd')  // Accept
      {
        tone(tonepin, 2400, 30);
        GAMEMINUTES = ((time[0] * 600) + (time[1] * 60) + (time[2] * 10) + (time[3]));
        break;
      }
      if (var == 'c')  // Cancel or Back Button :')
      {
        tone(tonepin, 2400, 30);
        goto menu1;
      }
    }
    tone(tonepin, 2400, 30);
    cls();
  }
  //BOMB TIME
  if (sdStatus || saStatus) {

menu2:
    cls();
    lcd.print(BOMB_TIME);
    delay(100);
    lcd.setCursor(0, 1);
    lcd.print(ZERO_MINUTES);
    lcd.cursor();
    lcd.blink();
    lcd.setCursor(0, 1);
    for (int i = 0; i < 2; i++) {
      while (1) {
        byte varu = getRealNumber();
        if (varu != 11) {
          time[i] = varu;
          lcd.print(varu);
          tone(tonepin, 2400, 30);
          break;
        }
      }
    }
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(13, 1);
    lcd.print("ok?");
    //zona donde pasamos los items a
    //redibujar
    while (1) {
      var = keypad.waitForKey();
      if (var == 'd')  //
      {
        tone(tonepin, 2400, 30);
        BOMBMINUTES = ((time[0] * 10) + (time[1]));
        break;
      }
      if (var == 'c')  // Cancel or Back Button :')
      {
        tone(tonepin, 2400, 30);
        goto menu2;
      }
    }
    tone(tonepin, 2400, 30);
    cls();
  }
  cls();
  //ARMING TIME
  if (sdStatus || doStatus || saStatus) {

menu3:
    cls();
    lcd.print(ARM_TIME);
    delay(100);
    lcd.setCursor(0, 1);
    lcd.print(ZERO_SECS);
    lcd.cursor();
    lcd.blink();
    lcd.setCursor(0, 1);
    for (int i = 0; i < 2; i++) {
      while (1) {
        byte varu = getRealNumber();
        if (varu != 11) {
          time[i] = varu;
          lcd.print(varu);
          tone(tonepin, 2400, 30);
          break;
        }
      }
    }
    lcd.noCursor();
    lcd.noBlink();
    lcd.setCursor(13, 1);
    lcd.print("ok?");

    //zona donde pasamos los items a
    //redibujar
    while (1) {
      var = keypad.waitForKey();
      if (var == 'd')  // Accept
      {
        tone(tonepin, 2400, 30);
        ACTIVATESECONDS = ((time[0] * 10) + (time[1]));
        break;
      }
      if (var == 'c')  // Cancel or Back Button :')
      {
        tone(tonepin, 2400, 30);
        goto menu3;
      }
    }
    tone(tonepin, 2400, 30);
    cls();
  }
  //sound??
  if (sdStatus || saStatus || doStatus) {
    cls();
    lcd.print(ENABLE_SOUND);
    lcd.setCursor(0, 1);
    lcd.print(YES_OR_NOT);

    while (1) {
      var = keypad.waitForKey();
      if (var == 'a') {
        soundEnable = true;
        tone(tonepin, 2400, 30);
        break;
      }

      if (var == 'b') {
        soundEnable = false;
        tone(tonepin, 2400, 30);
        break;
      }
    }
  }
  //Activate RELAY at Terrorist game ends??? Boom!

  if (sdStatus || saStatus) {
    cls();
    lcd.print(ENABLE_RELAYPIN);
    lcd.setCursor(0, 1);
    lcd.print(YES_OR_NOT);
    while (1) {
      var = keypad.waitForKey();
      if (var == 'a') {
        relayEnable = true;
        tone(tonepin, 2400, 30);
        break;
      }
      if (var == 'b') {
        relayEnable = false;
        tone(tonepin, 2400, 30);
        break;
      }
    }
  }
  //You Want a password enable-disable game?
  if (sdStatus || saStatus) {
    cls();
    lcd.print(ENABLE_CODE);
    lcd.setCursor(0, 1);
    lcd.print(YES_OR_NOT);

    while (1) {
      var = keypad.waitForKey();
      if (var == 'a') {
        tone(tonepin, 2400, 30);
        setNewPass();
        passwordEnable = true;
        break;
      }
      if (var == 'b') {
        tone(tonepin, 2400, 30);
        passwordEnable = false;
        break;
      }
    }
    tone(tonepin, 2400, 30);
  }
  //Continue the game :D
}






//This fuction compare codeInput[4] and password[4] variables
boolean comparePassword() {

  for (int i = 0; i < 4; i++) {
    if (codeInput[i] != password[i]) return false;
  }
  return true;
}

//Set the password variable
void setCode() {

  lcd.setCursor(0, 1);
  for (int i = 0; i < 4; i++) {
    while (1) {
      var = getNumber();
      if (var != 'x') {
        codeInput[i] = var;

        if (i != 0) {
          lcd.setCursor(i - 1, 1);
          lcd.print("*");
          lcd.print(var);
        } else {
          lcd.print(var);
        }
        tone(tonepin, 2400, 30);
        break;
      }
    }
  }
}
void setCodeTime() {

  timeCalcVar = millis();

  for (int i = 0; i < 4; i++) {
    while (1) {
      if (ACTIVATESECONDS * 1000 + timeCalcVar - millis() <= 100) {
        codeInput[i] = 'x';
        break;
      }

      lcd.setCursor(11, 0);
      printTimeDom(ACTIVATESECONDS * 1000 + timeCalcVar - millis(), false);

      var = getNumber();
      if (var != 'x') {
        codeInput[i] = var;

        if (i != 0) {
          lcd.setCursor(i - 1, 1);
          lcd.print("*");
          lcd.print(var);
        } else {
          lcd.print(var);
        }
        tone(tonepin, 2400, 30);
        break;
      }
    }
  }
}
void setPass() {
  lcd.setCursor(0, 1);

  for (int i = 0; i < 4; i++) {
    while (1) {
      var = getNumber();
      if (var != 'x') {
        password[i] = var;
        if (i != 0) {
          lcd.setCursor(i - 1, 1);
          lcd.print("*");
          lcd.print(var);
        } else {
          lcd.print(var);
        }
        tone(tonepin, 2400, 30);
        break;
      }
    }
  }
}

void setNewPass() {

  while (1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nova Senha");
    setPass();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Repita");

    setCode();

    if (comparePassword()) {

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Senha OK!");
      delay(2000);
      break;
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Erro!");
      if (soundEnable) tone(tonepin, errorTone, 200);
      delay(2000);
    }
  }
}
//Whait until a button is pressed is is a number return the number 'char' if not return x



char getNumber() {
  while (1) {
    var = keypad.getKey();
    if (var) {  //
      switch (var) {
        case 'a':
          return 'x';
          break;
        case 'b':
          return 'x';
          break;

        case 'c':
          return 'x';
          break;
        case 'd':
          return 'x';
          break;
        case '*':
          return 'x';
          break;
        case '#':
          return 'x';
          break;
        default:
          return var;
          break;
      }
    }
    return 'x';
  }
}

byte getRealNumber() {

  while (1) {
    var = keypad.waitForKey();

    if (var) {  //
      switch (var) {
        case '1':
          return 1;
          break;
        case '2':
          return 2;
          break;

        case '3':
          return 3;
          break;
        case '4':
          return 4;
          break;
        case '5':
          return 5;
          break;
        case '6':
          return 6;
        case '7':
          return 7;
          break;
        case '8':
          return 8;
          break;
        case '9':
          return 9;
          break;
        case '0':
          return 0;
          break;

        default:
          return 11;
          break;
      }
    }
    return 11;
  }
}







//void getConfig(){
//
////Check first time
//
//if (EEPROM.read(0)!= 1){
////write default config values
//
//
//
//}
//
////RELAY_TIME = EEPROM.read(1) * 1000 ;
//
//
//}

void drawBar(byte porcent) {
  //TODO: Optimize this code
  int box = (8 * porcent) / 10;
  lcd.setCursor(0, 1);
  while (box >= 5) {
    if (box >= 5) {
      lcd.write(4);
      box -= 5;
    }
  }
  switch (box) {
    case 0:
      break;
    case 1:
      lcd.write((uint8_t)0);
      break;
    case 2:
      lcd.write(1);
      break;
    case 3:
      lcd.write(2);
      break;
    case 4:
      lcd.write(3);
      break;
  }
}
void cls() {
  lcd.clear();
  lcd.setCursor(0, 0);
}

void printTime(unsigned long minutos, unsigned long aTiempo) {

  timeCalcVar = minutos - aTiempo / 60000;
  //Hours

  if (timeCalcVar / 60 == 0 && refresh) {
    lcd.clear();
    refresh = false;
    //delay(100);
    lcd.setCursor(3, 1);
    Serial.println("!!!!");
  }

  if (timeCalcVar / 60 >= 1) {

    if (timeCalcVar / 60 < 10) {
      lcd.setCursor(2, 1);
      lcd.print("0");
      lcd.print(timeCalcVar / 60);
    } else {
      lcd.print(timeCalcVar / 60);
    }

    lcd.print(":");
  }
  //minutes
  if (timeCalcVar % 60 < 10) {
    lcd.print("0");
    lcd.print(timeCalcVar % 60);
  } else {
    lcd.print(timeCalcVar % 60);
  }
  lcd.print(":");
  //seconds
  timeCalcVar = aTiempo / 1000;
  if (59 - (timeCalcVar % 60) < 10) {
    lcd.print("0");
    lcd.print(59 - (timeCalcVar % 60));
  } else {
    lcd.print(59 - (timeCalcVar % 60));
  }
  lcd.print(":");
  //this not mach with real time, is just a effect, it says 999 because millis%1000 sometimes give 0 LOL
  lcd.print(999 - (millis() % 1000));
}

void printTimeDom(unsigned long aTiempo, boolean showMillis) {
  //minutes
  if ((aTiempo / 60000) < 10) {
    lcd.print("0");
    lcd.print(aTiempo / 60000);
  } else {
    lcd.print(aTiempo / 60000);
  }
  lcd.print(":");
  //seconds
  if (((aTiempo / 1000) % 60) < 10) {
    lcd.print("0");
    lcd.print((aTiempo / 1000) % 60);
  } else {
    lcd.print((aTiempo / 1000) % 60);
  }
  if (showMillis) {
    lcd.print(":");
    //this not mach with real time, is just a effect, it says 999 because millis%1000 sometimes give 0 LOL
    lcd.print(999 - millis() % 1000);
  }
}

void startGameCount() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Pronto para Começar");
  lcd.setCursor(0, 1);
  lcd.print("Pressione Qualquer Tecla");
  keypad.waitForKey();  // se você pressionar um botão, o jogo começa

  cls();
  lcd.setCursor(1, 0);
  lcd.print("Iniciando Jogo");
  for (int i = 5; i > 0; i--) {  // INICIANDO CONTAGEM PARA O JOGO
    lcd.setCursor(5, 1);
    tone(tonepin, 2000, 100);
    lcd.print("EM ");
    lcd.print(i);
    delay(1000);
  }
  cls();
}


void checkArrows(byte i, byte maxx) {

  if (i == 0) {
    lcd.setCursor(15, 1);
    lcd.write(6);
  }
  if (i == maxx) {
    lcd.setCursor(15, 0);
    lcd.write(5);
  }
  if (i > 0 && i < maxx) {
    lcd.setCursor(15, 1);
    lcd.write(6);
    lcd.setCursor(15, 0);
    lcd.write(5);
  }
}

void activateRelay() {

  digitalWrite(RELAYPIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(RELAY_TIME);
  digitalWrite(RELAYPIN, LOW);
}

void explodeSplash() {
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  cls();
  delay(100);
  endGame = false;
  lcd.setCursor(1, 0);
  lcd.print("Terroristas Venceram");
  lcd.setCursor(4, 1);
  lcd.print("Fim de Jogo");
  for (int i = 200; i > 0; i--)  // este é o som de explosão de ultra alta definição xD
  {
    tone(tonepin, i);
    delay(20);
  }
  noTone(tonepin);
  if (relayEnable) {
    activateRelay();
  }
  delay(5000);
  cls();

  // código de fim
  lcd.print("Jogar Novamente?");
  lcd.setCursor(0, 1);
  lcd.print("A: Sim B: Não");
  while (1) {
    var = keypad.waitForKey();
    if (var == 'a') {
      tone(tonepin, 2400, 30);
      // Temos duas opções, buscar e destruir e sabotagem, então jogar novamente!
      if (sdStatus) {
        startGameCount();
        search();
      }
      if (saStatus) {
        saStatus = true;
        startGameCount();
        start = true;  // para definir iTime para millis() atual :D
        sabotage();
      }
    }
    if (var == 'b') {
      tone(tonepin, 2400, 30);
      menuPrincipal();

      break;
    }
  }
}

void failSplash() {
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  cls();
  delay(100);
  endGame = false;
  lcd.setCursor(1, 0);
  lcd.print("TEMPO ESGOTADO");
  lcd.setCursor(4, 1);
  lcd.print("FIM DE JOGO");
  for (int i = 200; i > 0; i--)  // este é o som de explosão de ultra alta definição xD
  {
    tone(tonepin, i);
    delay(20);
  }
  noTone(tonepin);
  if (relayEnable) {
    activateRelay();
  }
  delay(5000);
  cls();

  // código de fim
  lcd.print("Jogar Novamente?");
  lcd.setCursor(0, 1);
  lcd.print("A: Sim B: Não");
  while (1) {
    var = keypad.waitForKey();
    if (var == 'a') {
      tone(tonepin, 2400, 30);
      // Temos duas opções, buscar e destruir e sabotagem, então jogar novamente!
      if (sdStatus) {
        startGameCount();
        search();
      }
      if (saStatus) {
        saStatus = true;
        startGameCount();
        start = true;  // para definir iTime para millis() atual :D
        sabotage();
      }
    }
    if (var == 'b') {
      tone(tonepin, 2400, 30);
      menuPrincipal();

      break;
    }
  }
}

void disarmedSplash() {
  endGame = false;
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  if (sdStatus || saStatus) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("BOMBA DESARMADA");
    lcd.setCursor(3, 1);
    lcd.print("TERRORISTAS VENCEM");
    digitalWrite(GREENLED, HIGH);
    delay(5000);
    digitalWrite(GREENLED, LOW);
  }
  // código de fim
  lcd.clear();
  lcd.print("Jogar Novamente?");
  lcd.setCursor(0, 1);
  lcd.print("A: Sim B: Não");
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  while (1) {
    var = keypad.waitForKey();
    if (var == 'a') {
      tone(tonepin, 2400, 30);
      // Temos duas opções, buscar e destruir e sabotagem, então jogar novamente!
      if (sdStatus) {
        startGameCount();
        search();
      }
      if (saStatus) {
        saStatus = true;
        startGameCount();
        start = true;  // para definir iTime para millis() atual :D
        sabotage();
      }
    }
    if (var == 'b') {
      tone(tonepin, 2400, 30);
      menuPrincipal();
      break;
    }
  }
}

void search() {
  refresh = true;
  cls();
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  //SETUP INITIAL TIME
  int minutos = GAMEMINUTES - 1;
  unsigned long iTime = millis();  //  initialTime in millisec
  unsigned long aTime;
  //var='o';

  //Starting Game Code
  while (1) {  // this is the important code, is a little messy but works good.

    //If you fail disarm.
    if (endGame) {
      failSplash();
    }

    //Code for led blinking
    timeCalcVar = (millis() - iTime) % 1000;
    if (timeCalcVar >= 0 && timeCalcVar <= 50) digitalWrite(GREENLED, HIGH);
    if (timeCalcVar >= 90 && timeCalcVar <= 130) digitalWrite(GREENLED, LOW);

    lcd.setCursor(3, 0);
    lcd.print(GAME_TIME_TOP);
    aTime = millis() - iTime;
    lcd.setCursor(3, 1);

    //PRINT TIME ON LCD

    printTime(minutos, aTime);

    //###########################CHECKINGS##################

    //Check If Game End
    if (minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) failSplash();
    //Serial.println(keypad.getKey());
    //USED IN PASSWORD GAME
    if ('d' == keypad.getKey() && passwordEnable) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(ARMING_BOMB);
      delay(1000);  //a little delay to think in the password
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(ENTER_CODE);

      setCodeTime();  // we need to set the comparation variable first it writes on codeInput[]

      //then compare :D

      if (comparePassword()) destroy();
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print(CODE_ERROR);
      if (soundEnable) tone(tonepin, errorTone, 200);
      delay(500);
      cls();
    }
    //Check If Is Activating
    while (defusing && !passwordEnable) {
      digitalWrite(GREENLED, LOW);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(ARMING_BOMB);
      lcd.setCursor(0, 1);
      unsigned int percent = 0;
      unsigned long xTime = millis();  //start disabling time
      while (defusing) {
        keypad.getKey();
        percent = (millis() - xTime) / (ACTIVATESECONDS * 10);

        drawBar(percent);
        //check if game time runs out during the disabling
        aTime = millis() - iTime;
        Serial.println(millis() - xTime);
        if ((minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) || minutos - aTime / 60000 > 4000000000) {
          endGame = true;
        }
        timeCalcVar = (millis() - xTime) % 1000;

        if (timeCalcVar >= 0 && timeCalcVar <= 40) {
          digitalWrite(REDLED, HIGH);
          if (soundEnable) tone(tonepin, alarmTone1, 200);
        }
        if (timeCalcVar >= 480 && timeCalcVar <= 520) {
          if (soundEnable) tone(tonepin, alarmTone2, 200);
          digitalWrite(REDLED, LOW);
        }



        if (percent >= 100) {
          digitalWrite(GREENLED, LOW);
          destroy();  // jump to the next gamemode
        }
      }
      cls();
      digitalWrite(REDLED, LOW);
    }
  }
}

void destroy() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(BOMB_ARMED);
  delay(1000);
  int minutos = BOMBMINUTES - 1;
  unsigned long iTime = millis();
  unsigned long aTime;
  int largoTono = 50;

  //MAIN LOOP
  while (1) {

    //If you fail disarm.
    if (endGame) {
      explodeSplash();
    }

    //Led Blink

    timeCalcVar = (millis() - iTime) % 1000;
    if (timeCalcVar >= 0 && timeCalcVar <= 40) {
      digitalWrite(REDLED, HIGH);
      if (soundEnable) tone(tonepin, activeTone, largoTono);
    }
    if (timeCalcVar >= 180 && timeCalcVar <= 220) {
      digitalWrite(REDLED, LOW);
    }
    //Sound
    aTime = millis() - iTime;
    timeCalcVar = (millis() - iTime) % 1000;
    if (timeCalcVar >= 245 && timeCalcVar <= 255 && minutos - aTime / 60000 < 2 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (timeCalcVar >= 495 && timeCalcVar <= 510 && minutos - aTime / 60000 < 4 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (timeCalcVar >= 745 && timeCalcVar <= 760 && minutos - aTime / 60000 < 2 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) < 10) largoTono = 300;

    lcd.setCursor(1, 0);
    lcd.print(DETONATION_IN);
    //Passed Time

    lcd.setCursor(3, 1);

    ////////HERE ARE THE TWO OPTIONS THAT ENDS THE GAME///////////

    ////TIME PASED AWAY AND THE BOMB EXPLODES
    if (minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0)  // Check if game ends
    {
      explodeSplash();
    }
    //print time

    printTime(minutos, aTime);

    //// SECOND OPTION: YOU PRESS DISARMING BUTTON

    //IF IS A PASSWORD GAME

    if ('d' == keypad.getKey() && passwordEnable) {

      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print(DISARMING);
      delay(1000);  //a little delay to think in the password

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(ENTER_CODE);

      setCodeTime();  // we need to set the compare variable first

      //then compare :D

      if (comparePassword()) {
        disarmedSplash();
      }
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print(CODE_ERROR);
      if (soundEnable) tone(tonepin, errorTone, 200);
      delay(500);
      cls();
    }

    if (defusing && !passwordEnable)  // disarming bomb
    {
      lcd.clear();
      digitalWrite(REDLED, LOW);
      lcd.setCursor(3, 0);
      lcd.print(DISARM);
      lcd.setCursor(0, 1);
      unsigned int percent = 0;
      unsigned long xTime = millis();
      while (defusing) {
        keypad.getKey();
        //check if game time runs out during the disabling
        aTime = millis() - iTime;
        if ((minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) || minutos - aTime / 60000 > 4000000000) {
          endGame = true;
        }
        timeCalcVar = (millis() - xTime) % 1000;
        if (timeCalcVar >= 0 && timeCalcVar <= 20) {
          digitalWrite(GREENLED, HIGH);
          if (soundEnable) tone(tonepin, alarmTone1, 200);
        }
        if (timeCalcVar >= 480 && timeCalcVar <= 500) {
          if (soundEnable) tone(tonepin, alarmTone2, 200);
          digitalWrite(GREENLED, LOW);
        }
        unsigned long seconds = (millis() - xTime);
        percent = seconds / (ACTIVATESECONDS * 10);
        drawBar(percent);

        //BOMB DISARMED GAME OVER
        if (percent >= 100) disarmedSplash();
      }
      digitalWrite(REDLED, LOW);
      digitalWrite(GREENLED, LOW);
      cls();
    }
  }
}



void sabotage() {
  endGame = false;
  refresh = true;
  cls();
  digitalWrite(REDLED, LOW);
  digitalWrite(GREENLED, LOW);
  //SETUP INITIAL TIME
  int minutos = GAMEMINUTES - 1;


  if (start) {
    iTime = millis();  //  initialTime of the game, use this because sabotage mode goes can return to sabotage()
    start = false;
  }

  unsigned long aTime;

  //Starting Game Code
  while (1) {  // this is the important code, is a little messy but works good.

    if (endGame) {
      failSplash();
    }
    //Code for led blinking
    timeCalcVar = (millis() - iTime) % 1000;
    if (timeCalcVar >= 0 && timeCalcVar <= 50) digitalWrite(GREENLED, HIGH);
    if (timeCalcVar >= 90 && timeCalcVar <= 130) digitalWrite(GREENLED, LOW);

    lcd.setCursor(3, 0);
    lcd.print(GAME_TIME);
    aTime = millis() - iTime;
    lcd.setCursor(3, 1);

    //PRINT TIME ON LCD
    printTime(minutos, aTime);
    //###########################CHECKINGS##################

    //Check If Game End
    if (minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) {
      failSplash();
    }

    //USED IN PASSWORD GAME
    if ('d' == keypad.getKey() && passwordEnable) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(ARMING_BOMB);
      delay(1000);  //a little delay to think in the password
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(ENTER_CODE);

      setCodeTime();  // we need to set the compare variable first

      //then compare :D

      if (comparePassword()) {
        destroySabotage();
      }
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(CODE_ERROR);
      if (soundEnable) tone(tonepin, errorTone, 200);
      delay(500);
      cls();
    }

    //Check If Is Activating
    while (defusing && !passwordEnable) {
      keypad.getKey();
      cls();
      digitalWrite(GREENLED, LOW);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(ARMING_BOMB);
      lcd.setCursor(0, 1);
      unsigned int percent = 0;
      unsigned long xTime = millis();  //start disabling time
      while (defusing) {
        keypad.getKey();
        //check if game time runs out during the disabling
        aTime = millis() - iTime;
        if ((minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) || minutos - aTime / 60000 > 4000000000) endGame = true;

        timeCalcVar = (millis() - xTime) % 1000;

        if (timeCalcVar >= 0 && timeCalcVar <= 40) {
          digitalWrite(REDLED, HIGH);
          if (soundEnable) tone(tonepin, alarmTone1, 200);
        }
        if (timeCalcVar >= 480 && timeCalcVar <= 520) {
          if (soundEnable) tone(tonepin, alarmTone2, 200);
          digitalWrite(REDLED, LOW);
        }
        unsigned long seconds = millis() - xTime;
        percent = (seconds) / (ACTIVATESECONDS * 10);
        drawBar(percent);

        if (percent >= 100) {
          digitalWrite(GREENLED, LOW);
          destroySabotage();  // jump to the next gamemode
        }
      }
      cls();
      digitalWrite(REDLED, LOW);
    }
  }
}

void destroySabotage() {
  endGame = false;
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(BOMB_ARMED);
  delay(1000);
  int minutos = BOMBMINUTES - 1;
  unsigned long iTime = millis();
  unsigned long aTime;
  int largoTono = 50;

  //MAIN LOOP
  while (1) {

    //If you fail disarm.
    if (endGame) {
      explodeSplash();
    }

    //Led Blink

    timeCalcVar = (millis() - iTime) % 1000;
    if (timeCalcVar >= 0 && timeCalcVar <= 40) {
      digitalWrite(REDLED, HIGH);
      if (soundEnable) tone(tonepin, activeTone, largoTono);
    }
    if (timeCalcVar >= 180 && timeCalcVar <= 220) {
      digitalWrite(REDLED, LOW);
    }
    //Sound

    timeCalcVar = (millis() - iTime) % 1000;
    aTime = millis() - iTime;
    if (timeCalcVar >= 245 && timeCalcVar <= 255 && minutos - aTime / 60000 < 2 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (timeCalcVar >= 495 && timeCalcVar <= 510 && minutos - aTime / 60000 < 4 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (timeCalcVar >= 745 && timeCalcVar <= 760 && minutos - aTime / 60000 < 2 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) < 10) largoTono = 300;

    lcd.setCursor(1, 0);
    lcd.print(DETONATION_IN);
    //Passed Time

    lcd.setCursor(3, 1);

    ////////HERE ARE THE TWO OPTIONS THAT ENDS THE GAME///////////

    ////TIME PASED AWAY AND THE BOMB EXPLODES
    if (minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0)  // Check if game ends
    {
      failSplash();
    }
    //print time
    printTime(minutos, aTime);

    //// SECOND OPTION: YOU PRESS DISARMING BUTTON

    //IF IS A PASSWORD GAME

    if ('d' == keypad.getKey() && passwordEnable) {

      cls();
      digitalWrite(REDLED, LOW);
      digitalWrite(GREENLED, HIGH);
      lcd.print(DISARMING);
      delay(1000);  //a little delay to think in the password

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(ENTER_CODE);

      setCodeTime();  // we need to set the compare variable first

      //then compare :D

      if (comparePassword()) {
        sabotage();
      }
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(CODE_ERROR);
      if (soundEnable) tone(tonepin, errorTone, 200);
      delay(500);
      cls();
    }

    if (defusing && !passwordEnable)  // disarming bomb
    {
      lcd.clear();
      digitalWrite(REDLED, LOW);
      lcd.setCursor(3, 0);
      lcd.print(DISARM);
      lcd.setCursor(0, 1);
      unsigned int percent = 0;
      unsigned long xTime = millis();
      while (defusing) {
        keypad.getKey();
        //check if game time runs out during the disabling
        aTime = millis() - iTime;
        if ((minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) || minutos - aTime / 60000 > 4000000000) {
          endGame = true;
        }

        timeCalcVar = (millis() - xTime) % 1000;
        if (timeCalcVar >= 0 && timeCalcVar <= 20) {
          digitalWrite(GREENLED, HIGH);
          if (soundEnable) tone(tonepin, alarmTone1, 200);
        }
        if (timeCalcVar >= 480 && timeCalcVar <= 500) {
          if (soundEnable) tone(tonepin, alarmTone2, 200);
          digitalWrite(GREENLED, LOW);
        }
        unsigned long seconds = (millis() - xTime);
        percent = seconds / (ACTIVATESECONDS * 10);
        drawBar(percent);

        //BOMB DISARMED RETURN TO SABOTAGE
        if (percent >= 100) {
          cls();
          lcd.print("Bomb Disarmed");
          delay(1000);
          sabotage();
        }
      }
      digitalWrite(REDLED, LOW);
      digitalWrite(GREENLED, LOW);
      cls();
    }
  }
}














void domination() {

  //SETUP INITIAL TIME
  int minutos = GAMEMINUTES - 1;
  boolean showGameTime = true;
  unsigned long a;
  unsigned long iTime = millis();  //  initialTime in millisec
  unsigned long aTime;

  team = 0;
  iZoneTime = 0;
  aTime = 0;
  redTime = 0;
  greenTime = 0;

  int largoTono = 50;
  // 0 = neutral, 1 = green team, 2 = red team
  a = millis();
  //Starting Game Code
  while (1)  // this is the important code, is a little messy but works good.
  {
    if (endGame) {
      gameOver();
    }

    keypad.getKey();
    aTime = millis() - iTime;
    //Code for led blinking
    timeCalcVar = (millis() - iTime) % 1000;
    if (timeCalcVar >= 0 && timeCalcVar <= 40) {
      if (team == 1) digitalWrite(GREENLED, HIGH);
      if (team == 2) digitalWrite(REDLED, HIGH);
    }
    if (timeCalcVar >= 50 && timeCalcVar <= 100) {
      if (team == 1) digitalWrite(GREENLED, LOW);
      if (team == 2) digitalWrite(REDLED, LOW);
    }
    // Sound!!! same as Destroy
    if (timeCalcVar >= 0 && timeCalcVar <= 40 && soundEnable) tone(tonepin, activeTone, largoTono);

    if (timeCalcVar >= 245 && timeCalcVar <= 255 && minutos - aTime / 60000 < 2 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (timeCalcVar >= 495 && timeCalcVar <= 510 && minutos - aTime / 60000 < 4 && soundEnable) tone(tonepin, activeTone, largoTono);
    if (timeCalcVar >= 745 && timeCalcVar <= 760 && minutos - aTime / 60000 < 2 && soundEnable) tone(tonepin, activeTone, largoTono);
    //Help to count 3 secs
    if (a + 2000 < millis()) {
      a = millis();
      showGameTime = !showGameTime;
      cls();
    }
    //THE NEXT TO METHODS SHOW "GAME TIME" AND "CONTROLED ZONE TIME" IT SHOWS 2 AND 2 SEC EACH

    if (showGameTime) {  //THE SECOND IS /2
      lcd.setCursor(3, 0);
      lcd.print("GAME TIME");
      lcd.setCursor(3, 1);
      printTime(minutos, aTime);
    } else if (!showGameTime) {

      lcd.setCursor(2, 0);
      if (team == 0) lcd.print("NEUTRAL ZONE");
      if (team == 1) lcd.print(" GREEN ZONE");
      if (team == 2) lcd.print("  RED ZONE");

      if (team > 0) {
        lcd.setCursor(3, 1);
        printTimeDom(millis() - iZoneTime, true);
      }
    }

    //###########################CHECKINGS##################

    //Check If Game End
    if (minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) {
      gameOver();
    }

    //Check If IS neutral
    while ((defusing || cancelando) && team > 0) {
      cls();
      if (team > 0) lcd.print("NEUTRALIZING...");
      lcd.setCursor(0, 1);
      unsigned int percent = 0;
      unsigned long xTime = millis();  //start disabling time
      while (defusing || cancelando) {
        //check if game time runs out during the disabling
        aTime = millis() - iTime;
        if ((minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) || minutos - aTime / 60000 > 4000000000) {
          endGame = true;
        }

        keypad.getKey();
        timeCalcVar = (millis() - xTime) % 1000;

        if (timeCalcVar >= 0 && timeCalcVar <= 20) {
          if (soundEnable) tone(tonepin, alarmTone1, 200);
        }
        if (timeCalcVar >= 480 && timeCalcVar <= 500) {
          if (soundEnable) tone(tonepin, alarmTone2, 200);
          digitalWrite(REDLED, LOW);
        }

        unsigned long seconds = millis() - xTime;
        percent = (seconds) / (ACTIVATESECONDS * 10);
        drawBar(percent);

        if (percent >= 100) {
          delay(1000);

          if (team == 1) {
            greenTime += millis() - iZoneTime;
            iZoneTime = 0;
          }
          if (team == 2) {
            redTime += millis() - iZoneTime;
            iZoneTime = 0;
          }
          team = 0;
          break;
        }
      }
      cls();
    }

    //Capturing red

    while (defusing && team == 0) {
      cls();
      if (team == 0) lcd.print(" CAPTURING ZONE");
      lcd.setCursor(0, 1);
      unsigned int percent = 0;
      unsigned long xTime = millis();  //start disabling time
      while (defusing) {
        keypad.getKey();
        //check if game time runs out during the disabling
        aTime = millis() - iTime;
        if ((minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) || minutos - aTime / 60000 > 4000000000) {
          endGame = true;
        }
        timeCalcVar = (millis() - xTime) % 1000;

        if (timeCalcVar >= 0 && timeCalcVar <= 20) {
          digitalWrite(REDLED, HIGH);
          if (soundEnable) tone(tonepin, alarmTone1, 200);
        }
        if (timeCalcVar >= 480 && timeCalcVar <= 500) {
          if (soundEnable) tone(tonepin, alarmTone2, 200);
          digitalWrite(REDLED, LOW);
        }

        unsigned long seconds = millis() - xTime;
        percent = (seconds) / (ACTIVATESECONDS * 10);
        drawBar(percent);

        if (percent >= 100) {
          digitalWrite(GREENLED, LOW);
          team = 2;
          iZoneTime = millis();
          delay(1000);
          break;
        }
      }
      cls();
      digitalWrite(REDLED, LOW);
    }

    //getting to green zone
    while (cancelando && team == 0) {
      cls();
      if (team == 0) lcd.print(" CAPTURING ZONE");
      lcd.setCursor(0, 1);
      unsigned int percent = 0;
      unsigned long xTime = millis();  //start disabling time
      while (cancelando) {
        keypad.getKey();
        //check if game time runs out during the disabling
        aTime = millis() - iTime;
        if ((minutos - aTime / 60000 == 0 && 59 - ((aTime / 1000) % 60) == 0) || minutos - aTime / 60000 > 4000000000) {
          endGame = true;
        }
        timeCalcVar = (millis() - xTime) % 1000;

        if (timeCalcVar >= 0 && timeCalcVar <= 20) {
          digitalWrite(GREENLED, HIGH);
          if (soundEnable) tone(tonepin, alarmTone1, 200);
        }
        if (timeCalcVar >= 480 && timeCalcVar <= 500) {
          if (soundEnable) tone(tonepin, alarmTone2, 200);
          digitalWrite(GREENLED, LOW);
        }

        unsigned long seconds = millis() - xTime;
        percent = (seconds) / (ACTIVATESECONDS * 10);
        drawBar(percent);

        if (percent >= 100) {
          digitalWrite(GREENLED, LOW);
          team = 1;
          iZoneTime = millis();
          delay(1000);
          break;
        }
      }
      cls();
      digitalWrite(GREENLED, LOW);
    }
  }
}

void gameOver() {

  if (team == 1) greenTime += millis() - iZoneTime;
  if (team == 2) redTime += millis() - iZoneTime;
  digitalWrite(GREENLED, LOW);
  digitalWrite(REDLED, LOW);
  while (!defusing) {
    keypad.getKey();
    if (defusing) {
      keypad.getKey();
      break;
    }
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("TIME OVER!");
    lcd.setCursor(0, 1);

    //check who team win the base
    if (greenTime > redTime) {
      //greenteam wins
      lcd.print(" GREEN TEAM WINS");
      digitalWrite(GREENLED, HIGH);
    } else {
      //redteam wins
      lcd.print("  RED TEAM WINS");
      digitalWrite(REDLED, HIGH);
    }
    delay(3000);
    keypad.getKey();
    if (defusing) {
      keypad.getKey();
      break;
    }
    cls();
    lcd.print("Red Time:");
    lcd.setCursor(5, 1);
    printTimeDom(redTime, false);
    delay(3000);
    keypad.getKey();
    if (defusing) {

      break;
    }
    cls();
    lcd.print("Green Time:");
    lcd.setCursor(5, 1);
    printTimeDom(greenTime, false);
    delay(3000);
    keypad.getKey();
    if (defusing) {
      keypad.getKey();
      break;
    }
  }
  cls();
  delay(100);
  lcd.print("Play Again?");
  lcd.setCursor(0, 1);
  lcd.print("A : Yes B : No");
  while (1) {
    var = keypad.waitForKey();
    if (var == 'a') {
      tone(tonepin, 2400, 30);
      cls();
      domination();
      break;
    }
    if (var == 'b') {
      tone(tonepin, 2400, 30);
      menuPrincipal();
      break;
    }
  }
}
