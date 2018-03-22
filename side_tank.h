/***************************************************************************
*            side_tank.h
*
*  These file is used to describe the side_tank of a bi-dimensional 2-level
*  pyramid of watertanks. This is a tank with a costant input and an output
*  that will flow into the bottom watertank.
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

#include <ariadne.h>

namespace Ariadne {

  /*
  * Function returning the desired automata. It takes in input the waterlevel,
  * the two upper_waterlevel, the upper_valvelevel, the two upper_output_flow
  * and its internal_output_flow.
  */
  HybridIOAutomaton getSideTank(
    RealVariable internal_waterlevel,
    RealVariable upper_valvelevel,
    RealVariable lower_valvelevel,
    RealParameter upper_input_flow,
    RealParameter lower_output_flow,
    int progressive){

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
