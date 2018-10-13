//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Flash Task to output something to the LED Matrix.
#include "Tasks_Flash.hpp"
#include "Debug.hpp"

#include <LEDMatrixDriver.hpp>
#include <EEPROM.h>
#include <functional>
#include <cassert>
#include <set>

namespace Tasks {

const int Flash::LEDMATRIX_WIDTH = 31;
const int Flash::LEDMATRIX_HEIGHT = 7;
const int Flash::LEDMATRIX_SEGMENTS = 4;
const int Flash::LEDMATRIX_INTENSITY = 5;
const int Flash::LEDMATRIX_CS_PIN = 16;

//! Initializes the LED Matrix display.
Flash::Flash() :
   Task(100 /*ms*/, TASK_FOREVER, std::bind(&Flash::execute, this)),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);
   EEPROM.begin(512);
   setDefaultPattern();
   loadPattern(1);
}

void Flash::setDefaultPattern() {

  // Pattern 0
  for (int i = 0; i < 128; i++) {
    EEPROM.write(i, i);
  }

  // Pattern 1
  for (int i = 128; i < 192; i++) {
    EEPROM.write(2 * i, 0xFF);
    EEPROM.write(2 * i + 1, 0x00);
  }

  EEPROM.commit();
}

//! Update display
void Flash::execute()
{
  static uint8_t pattern = 0;
  static uint8_t intensity = 0;
  static uint8_t delta = 1;
  
  if (intensity == 0) {
    loadPattern(pattern);
    pattern = (pattern + 1) % 2;
  }
  
  m_lmd.setIntensity(intensity);
  intensity += delta;

  if ((intensity >= 10) || (intensity <= 0)) {
    delta = -delta;
  }
    
}

void Flash::loadPattern(int pattern_no) {  
  // Pattern number. Each pattern needs 4 * 8 * 4 = 128 bytes.
  const int position = 0; // Position in stack of displays. Each display has 4 * 8 = 32 bytes.
  uint8_t data[8]; 
  
  m_lmd.clear();
  for (int block = 0; block < 4; block++) {

    for (int row = 0; row < 8; row++) {
      data[row] = EEPROM.read(128 * pattern_no + 32 * position + 8 * block + row);
    }

    displayBlock(block, data);
  }

  m_lmd.display();
}

void Flash::displayBlock(int block, uint8_t *data) {
  
  for (int row = 0; row < 8; row++) {
    uint8_t byte = data[row];
    
    for (int x = 0; x < 8; x++) {
      m_lmd.setPixel(8 * (3 - block) + x, row, byte & (0x1 << (7 - x)));
    }
  
  }
  
}



} // namespace Tasks

