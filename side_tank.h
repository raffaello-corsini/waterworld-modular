/***************************************************************************
*            side_tank.h
*
*  These file is used to describe the side_tank of a bi-dimensional pyramid
*  of watertanks.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Library General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*
#ifndef SIDE_TANK_H_
#define SIDE_TANK_H_

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_
*/

// Including this library to work with strings.
// #include <string>

#include <ariadne.h>

namespace Ariadne {

  /*
  *  Function returning the desired automata. It takes in input a string which
  *  is the label of the automata, the waterlevel, the two upper_waterlevel,
  *  the upper_valvelevel, the two upper_output_flow and its internal_output_flow.
  */


  // Posso passargli un automa? O meglio passargli solo la variabile?
  HybridIOAutomaton getSideTank(
    String label,
    RealVariable internal_waterlevel,
    RealVariable upper_valvelevel,
    RealVariable lower_valvelevel,
    RealParameter upper_input_flow,
    RealParameter lower_output_flow,
    // This int represents the number of this component.
    int progressive
  )
  {
    // Creation of the automaton, with the assigned label.
    HybridIOAutomaton tank(label);

    // Adding the input/output vars.
    tank.add_input_var(upper_valvelevel);
    tank.add_output_var(internal_waterlevel);

    // Creation of the location.
    DiscreteLocation side_tank_flow("flow" + Ariadne::to_string(progressive));
    // Adding the location to the automaton.
    tank.new_mode(side_tank_flow);

    // Setting the dynamics
    tank.set_dynamics(side_tank_flow, internal_waterlevel,
      - lower_output_flow *  ( lower_valvelevel / 2 ) * internal_waterlevel + upper_input_flow * upper_valvelevel);

      return tank;
    }

  }
