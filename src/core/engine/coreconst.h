/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
  * @file coreconst.h
  *
  * @brief Set of constants that define the game mechanics
  *
  * Unlikely that they will ever change, but just in case it helps to have them
  * at one place.
  */

#ifndef CORECONST_H
#define CORECONST_H

/**
  * @brief max number of lifes in the game
  *
  * limit is set, as more than 6 can't be shown in the game currently
  */
#define MAX_LIFE_TOKEN 6

/**
  * @brief Max number of damage token a robot can receive
  *
  * When a Robot get 10 damage token it will be destroyed
  */
#define MAX_DAMAGE_TOKEN 10

/**
  * @brief Max size of the Participant card deck
  */
#define MAX_DECK_SIZE 9

/**
  * @brief Max size of the Participant program deck
  */
#define MAX_PROGRAM_SIZE 5

/**
  * @brief Max number of cards dealt per round
  */
#define CARDS_PER_ROUND 9

/**
 * @brief size of a single tile in the theme svg file
 *
 */
const int DEFAULT_TILE_SIZE = 50;

#endif // CORECONST_H
