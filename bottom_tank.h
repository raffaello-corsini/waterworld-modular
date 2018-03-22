/***************************************************************************
*            bottom_tank.h
*
*  These file is used to describe the bottom_tank of a bi-dimensional pyramid
*  of watertanks. It takes as inputs two flows from the upper tanks and it
*  loses water from the bottom.
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
  * Function returning the desired automata. It takes in input
  * the waterlevel, the two upper_waterlevel, the upper_valvelevel,
  * the two upper_output_flow and its internal_output_flow.
  */
  HybridIOAutomaton getBottomTank(
    RealVariable internal_waterlevel,
    RealVariable upper_left_waterlevel,
    RealVariable upper_right_waterlevel,
    RealVariable upper_valvelevel,
    RealParameter upper_left_output_flow,
    RealParameter upper_right_output_flow,
    RealParameter lower_output_flow,
    int progressive){

      // Conversion of the progressive integer to a String.
      String number = Ariadne::to_string(progressive);

      // Creation of the automaton, with a progressive label.
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
