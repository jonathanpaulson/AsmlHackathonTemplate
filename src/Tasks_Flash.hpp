//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Flash Task

#ifndef __Tasks_Flash__
#define __Tasks_Flash__

#include "Facilities_MeshNetwork.hpp"
#include "Debug.hpp"

#include <LEDMatrixDriver.hpp>

#include <functional>
#include <vector>
#include <set>

// Forward declarations.
namespace Tasks {

class Flash : public Task
{
public:
   explicit Flash();
   ~Flash() {};

   // Disallow copy-ing
	Flash(const Flash& other) = delete;
	Flash(Flash&& other) = delete;
	Flash& operator=(const Flash& other) = delete;

   void execute();

private:
   static const int SQ;

   static const int LEDMATRIX_WIDTH;
   static const int LEDMATRIX_HEIGHT;
   static const int LEDMATRIX_SEGMENTS;
   static const int LEDMATRIX_INTENSITY;
   static const int LEDMATRIX_CS_PIN;

   LEDMatrixDriver m_lmd;

   vector<vector<vector<int>>> m_pattern;
   
   void setDefaultPattern();
   void loadPattern(int pattern_no);
   void displayBlock(int block, uint8_t *data);
 };

} // namespace Tasks

#endif //  __Tasks_Flash__
