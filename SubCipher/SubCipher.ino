#include <Arduino.h>
#include <inttypes.h>
#include "C3SPI/C3SPI.h"
#include "MCP/MCP_IO.h"
#include <Adafruit_NeoPixel.h>
#include <vector>
#include <string>
#include <atomic>
#include <algorithm>
#include <queue>
#include <mutex>
#include "IRTXRX\IR_TX.h"
#include "IRTXRX\IR_RX.h"
#include "GameEvent.h"
#include "GameEventPublisher.h"
#include "Keyboard.h"
#include "MagicLEDs.h"
#include "CrystalPedestal.h"
#include "esp_wifi.h"

const int slaveSelect = 21;

const int sck_pin = 7;
const int mosi_pin = 8;
const int miso_pin = 6;

const int total_io_count = 32;
const int button_count = 26;
const int led_count = 26;

const int ir_out_pin = 2;
const int ir_in_pin = 10;
const int ir_rx_que_len = 4;

const int answer_pin = 31;

std::string answer;
std::string playerGuess;
const uint32_t unpressedColor = 0x00606060; //Grey
const uint32_t pressedColor = 0x0000ff00; //Pure Green
const uint32_t secondPressedColor = 0x000000ff; //Pure blue
const uint32_t wrongKeyColor = 0x00ff0000; //rure red
uint32_t idleColor = 0x00ff0000; //Red
MagicType awardMagicType;
int awardMagicQty = 50;
std::queue<char> key_que;
std::mutex mu_key_que;


C3SPI mySpi(HSPI);
MCP_IO mcp_io(mySpi, slaveSelect);
const int led_pin = 3;
MagicLEDs magicLeds(led_pin);
const int delay_ms = 20;
std::atomic<int> timer = 0;


GameEventPublisher publisher;
Keyboard keyboard(mcp_io, publisher, {'P', 'U', 'M', 'K', 'I', 'N', 'H', 'B', 'O', 'L', 'D', 'T'});
CrystalPedestal crystalPedestal(publisher, ir_out_pin, ir_in_pin, RMT_CHANNEL_1, RMT_CHANNEL_2);



enum GameState 
{
  AcceptingKeys,
  DisplayFailure,
  DisplaySuccess,
  Idle
};

std::atomic<GameState> gameState = Idle;


void addKeyToQue(char key)
{
  std::lock_guard<std::mutex> locker(mu_key_que);
  key_que.push(key);
}

char getKeyFromQue()
{
  char ret = 0;
  std::lock_guard locker(mu_key_que);
  if (!key_que.empty())
  {
    ret = key_que.front();
    key_que.pop();
  }
  return ret;
}

void clearKeyQue()
{
  std::lock_guard locker(mu_key_que);
  while (!key_que.empty()) key_que.pop();
}


void activatePuzzleFailureMode()
{
  playerGuess.clear();
  clearKeyQue();
  Serial.println("Failure Mode");
  gameState = DisplayFailure;
  magicLeds.runFailureAnimation();
  timer = 150;
}

void activatePuzzleSuccessMode()
{
  playerGuess.clear();
  clearKeyQue();
  Serial.println("Success Mode");
  gameState = DisplaySuccess;
  magicLeds.runSuccessAnimation();
  crystalPedestal.addMagic(awardMagicQty, awardMagicType);
  timer = 150;
}

void activatePuzzleIdleMode()
{
  playerGuess.clear();
  clearKeyQue();
  Serial.println("Idle Mode");
  gameState = Idle;
  magicLeds.cancelAnimation();
  magicLeds.setAllLetters(idleColor);
}

void activatePuzzleAcceptKeysMode()
{
  playerGuess.clear();
  clearKeyQue();
  magicLeds.cancelAnimation();
  Serial.println("Accepting Keys Mode");
  gameState = GameState::AcceptingKeys;
  magicLeds.setAllLetters(unpressedColor);
}



void onKeyPressed(char key)
{
  Serial.print("Key pressed: [");
  Serial.print(key);
  Serial.println("]");

  addKeyToQue(key);

}

void onCrystalPresent()
{
  if (gameState == GameState::Idle)
  {
    activatePuzzleAcceptKeysMode();
  }
}

void onCrystalRemoved()
{
  if (gameState != GameState::Idle)
  {
    activatePuzzleIdleMode();
  }
}

void onPuzzleSuccess()
{
  if (gameState != GameState::DisplaySuccess)
  {
    Serial.println("GameState: [DisplaySuccess]");
    gameState = GameState::DisplaySuccess;
    magicLeds.runSuccessAnimation();
    
  }
}

void onPuzzleFailure()
{
  if (gameState != GameState::DisplayFailure)
  {
    Serial.println("GameState: [DisplayFailure]");
    gameState = GameState::DisplayFailure;
    magicLeds.runFailureAnimation();
  }
}

void setupGameEventDispatch()
{
  publisher.listen<GameEvent>([&](GameEvent gameEvent)
  {

    //This is on the GamePublishers thread! 
    //I think everything is shielded though

    //TODO you have too many events here
    //success and failure are now gamestates


    switch(gameEvent.eventType)
    {
    case GameEvent::KeyPressed:
      onKeyPressed(gameEvent.value);
      break;
    case GameEvent::CrystalPresent:
      onCrystalPresent();
      break;
    case GameEvent::CrystalRemoved:
      onCrystalRemoved();
      break;
    case GameEvent::PuzzleSuccess:
      onPuzzleSuccess();
      break;
    case GameEvent::PuzzleFailure:
      break;    
    }
  });
}


void setup() 
{
  esp_wifi_stop();
  Serial.begin(115200);
  while(!Serial);
  vTaskDelay(6000/portTICK_PERIOD_MS);
  
  Serial.println("SubCipher startup");
  
  pinMode(led_pin, OUTPUT);
  pinMode(slaveSelect, OUTPUT);
  pinMode(sck_pin, OUTPUT);
  pinMode(miso_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);

  
  mySpi.begin(sck_pin, miso_pin, mosi_pin);  
  mcp_io.pullupMode(answer_pin, 1);
  


  if (mcp_io.digitalRead(answer_pin))
  {
    answer = "PUMPKIN";
    awardMagicType = Red;
  }
  else
  {
    answer = "HUMBOLDT";
    awardMagicType = Blue;
    idleColor = 0x00000080;
  }
  
  Serial.print("Answer: [");
  Serial.print(answer.c_str());
  Serial.print("] awarded magic type: [");
  Serial.print(magicTypeToString(awardMagicType).c_str());
  Serial.println("]");
  

  publisher.startPublisher();
  magicLeds.startLEDs();
  activatePuzzleIdleMode();
  setupGameEventDispatch();
  crystalPedestal.begin();
  

  Serial.println("SubCipher startup Done");
}

bool isAnswerOk()
{
  int count = std::min(answer.size(), playerGuess.size());
  for (int i = 0; i < count; ++i)
  {
    if (playerGuess[i] != answer[i])
    {
      return false;
    }
  }
  return playerGuess.size() <= answer.size();
}

void loop() 
{ 
  keyboard.loop();
  crystalPedestal.loop();
  char key = 0;

  switch(gameState)
  {
  case AcceptingKeys:
    key = getKeyFromQue();
    if (key != 0)
    {
      uint32_t pressColor = playerGuess.find(key) == std::string::npos ? pressedColor : secondPressedColor;
      
      playerGuess.push_back(key);
      if (!isAnswerOk())
      {
        magicLeds.setLetterColor(key, wrongKeyColor);
        activatePuzzleFailureMode();    
      }
      else
      {
        magicLeds.setLetterColor(key, pressColor);
        if (playerGuess.size() >= answer.size())
        {
          activatePuzzleSuccessMode();
        }
      }
      

      
    }
    break;
  case DisplayFailure:
  case DisplaySuccess:

    if (timer == 0) 
    {
      activatePuzzleIdleMode();
    }
    break;
  default:
  case Idle:
    break;
  }

  /*Serial.print("MCP_GPIO_0: [");
  Serial.print(mcp_io.digitalRead(0));
  Serial.print("]  MCP_GPIO_31: []");
  Serial.print(mcp_io.digitalRead(31));
  Serial.println("]");*/

  vTaskDelay(delay_ms / portTICK_PERIOD_MS);
  if (timer > 0) --timer;
}

