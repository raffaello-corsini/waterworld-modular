/***************************************************************************
*            bottom_tank.h
*
*  These file is used to describe the bottom_tank of a bi-dimensional pyramid
*  of watertanks.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
#ifndef BOTTOM_TANK_H_
#define BOTTOM_TANK_H_
*/

#include <ariadne.h>

namespace Ariadne {

  /*
  *  Function returning the desired automata. It takes in input a string which
  *  is the label of the automata, the waterlevel, the two upper_waterlevel,
  *  the upper_valvelevel, the two upper_output_flow and its internal_output_flow.
  */

  HybridIOAutomaton getBottomTank(
    RealVariable internal_waterlevel,
    RealVariable upper_left_waterlevel,
    RealVariable upper_right_waterlevel,
    RealVariable upper_valvelevel,
    RealParameter upper_left_output_flow,
    RealParameter upper_right_output_flow,
    RealParameter lower_output_flow,
    int progressive)
    {

      // Conversion of the progressive integer to a String.
      String number = Ariadne::to_string(progressive);

      // Creation of the automaton, with the assigned label.
      HybridIOAutomaton tank("tank" + number);

      // Adding the input/output vars.
      tank.add_input_var(upper_valvelevel);
      tank.add_input_var(upper_left_waterlevel);
      tank.add_input_var(upper_right_waterlevel);

      tank.add_output_var(internal_waterlevel);

      // Creation of the location.
      DiscreteLocation bottom_tank_flow("flow" + number);
      // Adding the location to the automaton.
      tank.new_mode(bottom_tank_flow);

      // Setting the dynamics
      tank.set_dynamics(bottom_tank_flow, internal_waterlevel,
        // What goes out.
        - lower_output_flow * internal_waterlevel // * valveLevelEnd
        // What comes in from the upper left tank.
        + upper_left_output_flow * ( upper_valvelevel / 2 ) * upper_left_waterlevel
        // What comes in from the upper right tank.
        + upper_right_output_flow * ( upper_valvelevel / 2 ) * upper_right_waterlevel);

        return tank;
      }

    }
