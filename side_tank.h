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
#ifndef SIDE_TANK_H_
#define SIDE_TANK_H_

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_
*/

#include <ariadne.h>

namespace Ariadne {

  /*
  *  Function returning the desired automata. It takes in input a string which
  *  is the label of the automata, the waterlevel, the two upper_waterlevel,
  *  the upper_valvelevel, the two upper_output_flow and its internal_output_flow.
  */

  // Posso passargli un automa? O meglio passargli solo la variabile?
  HybridIOAutomaton getSideTank(
    RealVariable internal_waterlevel,
    RealVariable upper_valvelevel,
    RealVariable lower_valvelevel,
    RealParameter upper_input_flow,
    RealParameter lower_output_flow,
    int progressive)
    {

      // Conversion of the progressive integer to a String.
      String number = Ariadne::to_string(progressive);

      // Creation of the automaton, with the assigned label.
      HybridIOAutomaton tank("tank" + number);

      // Adding the input/output vars.
      tank.add_input_var(upper_valvelevel);
      tank.add_output_var(internal_waterlevel);

      // Creation of the location.
      DiscreteLocation side_tank_flow("flow" + number);
      
      // Adding the location to the automaton.
      tank.new_mode(side_tank_flow);

      // Setting the dynamics
      tank.set_dynamics(side_tank_flow, internal_waterlevel,
        - lower_output_flow *  ( lower_valvelevel / 2 ) * internal_waterlevel + upper_input_flow * upper_valvelevel);

        return tank;
      }

    }
