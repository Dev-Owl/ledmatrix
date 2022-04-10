#include <FastLED.h>
#include <Arduino.h>

#define NUM_LEDS 256 //It must be dividable by 16
#define DATA_PIN 3
#define MIN_INITIAL_CELLS 50
#define MAX_INITIAL_CELLS 150
#define LIVE_CELL_VALUE 50
#define MAX_GENERATIONS 60

struct Grid {
  int x;
  int y;
};

void setRandomStart();
Grid arrayToMatrix(int i);
int matrixToArray(int x, int y);
void cellUpdate();

CRGB leds[NUM_LEDS];
unsigned long lastFrame = 0;
int fps = 2;
byte generations = 0;



int matrixToArray(int x, int y) {
  if (y % 2 == 0) {
    x = 15 - x;
  }
  return x + (16 * y);
}

Grid arrayToMatrix(int i) {
  int y = i / 16;
  int x = 0;
  if (y % 2 == 0) {
    x = 15 - (i - (y * 16));
  } else {
    x = (i - (y * 16));
  }
  return (Grid) {
    x,
    y
  };
}

void setRandomStart() {
  //Rest all LEDS to black/off
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB::Black;
  }
  FastLED.show();
  /*
    //Start with a walker
    leds[matrixToArray(1, 0)].green = LIVE_CELL_VALUE;
    leds[matrixToArray(2, 1)].green = LIVE_CELL_VALUE;
    leds[matrixToArray(0, 2)].green = LIVE_CELL_VALUE;
    leds[matrixToArray(1, 2)].green = LIVE_CELL_VALUE;
    leds[matrixToArray(2, 2)].green = LIVE_CELL_VALUE;
  */
  FastLED.show();


  //Get amount of starting cells between min and max
  long remainingCellsAlive = random(MIN_INITIAL_CELLS, MAX_INITIAL_CELLS);
  //Set cells to living with a random pattern
  while (remainingCellsAlive > 0)
  {
    long rndLed = random(0, NUM_LEDS);
    if (leds[rndLed].green != LIVE_CELL_VALUE) {
      remainingCellsAlive--;
      leds[rndLed].green = LIVE_CELL_VALUE;
    }
  }

  delay(1000);
}


void setup() {
  Serial.begin(9600);
  //Seed the random number by not connected pin
  randomSeed(analogRead(0));
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  setRandomStart();
  FastLED.show();
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastFrame >= (1000 / fps)) {
    Serial.println("frame start");
    lastFrame = currentMillis;
    cellUpdate();
    //stateChangeAnimation();
    FastLED.show();
    //checkLockedState();
    Serial.println("frame end");
    generations++;
  }
  if (generations >= MAX_GENERATIONS) {
    setRandomStart();
    generations = 0;
  }
}

void cellUpdate() {
  byte newWorld[NUM_LEDS];
  for (int i = 0; i < NUM_LEDS; ++i) {
    if (leds[i].green == LIVE_CELL_VALUE) {
      newWorld[i] = 1;
    }
    else {
      newWorld[i] = 0;
    }
  }
  for (int i = 0; i < NUM_LEDS; ++i) {
    int neighbours = 0;
    Grid currentCell = arrayToMatrix(i);
    if (currentCell.x > 0) {
      //WEST
      if (leds[matrixToArray(currentCell.x - 1, currentCell.y)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }

    if (currentCell.x < 15) {
      //EAST
      if (leds[matrixToArray(currentCell.x + 1, currentCell.y)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }

    if (currentCell.y > 0) {
      //NORTH
      if (leds[matrixToArray(currentCell.x, currentCell.y - 1)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }

    if (currentCell.y < 15) {
      //SOUTH
      if (leds[matrixToArray(currentCell.x, currentCell.y + 1)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }

    if (currentCell.y > 0 && currentCell.x > 0) {
      //North west
      if (leds[matrixToArray(currentCell.x - 1, currentCell.y - 1)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }
    if (currentCell.y > 0 && currentCell.x < 15) {
      //North east
      if (leds[matrixToArray(currentCell.x + 1, currentCell.y - 1)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }
    if (currentCell.y < 15 && currentCell.x > 0) {
      //South west
      if (leds[matrixToArray(currentCell.x - 1, currentCell.y + 1)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }

    if (currentCell.y < 15 && currentCell.x < 15) {
      //South east
      if (leds[matrixToArray(currentCell.x + 1, currentCell.y + 1)].green == LIVE_CELL_VALUE) {
        neighbours += 1;
      }
    }
    //Whatever has 3 neighbours will live
    if (neighbours == 3) {
      newWorld[i] = 1;
    } //if it doesnt have exactly two it will die
    else if (neighbours != 2) {
      newWorld[i] = 0;
    }
  }
  //Apply the new world to the LED matrix
  for (int i = 0; i < NUM_LEDS; ++i) {
    if (newWorld[i] == 1) {
      leds[i].green = LIVE_CELL_VALUE;
    }
    else {
      leds[i].green = 0;
    }

  }

}
