/***************************************************************************
*            valve_tank.h
*
*  These file is used to describe the valve of a watertank.
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
#ifndef VALVE_H_
#define VALVE_H_

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_
*/

// Including this library to work with strings.
// #include <string>

#include <ariadne.h>

namespace Ariadne {

  // Posso passargli un automa? O meglio passargli solo la variabile?
  HybridIOAutomaton getValve(
    String label,
    // Valve's opening time.
    RealParameter opening_time,
    // Valve's opening level.
    RealVariable valvelevel,
    // This int represents the number of this component.
    int progressive)
    {

      // 1. Automaton

      HybridIOAutomaton valve(label);

      // 2. Registration of the input/output variables

      valve.add_output_var(valvelevel);

      // 3 Registration of the input/internal events

      DiscreteEvent e_open("e_open_" + Ariadne::to_string(progressive));
      DiscreteEvent e_close("e_close_" + Ariadne::to_string(progressive));
      DiscreteEvent e_idle("e_idle_" + Ariadne::to_string(progressive));
      valve.add_input_event(e_open);
      valve.add_input_event(e_close);
      valve.add_internal_event(e_idle);

      // 4. Registration of the locations

      DiscreteLocation idle("idle_" + Ariadne::to_string(progressive));
      DiscreteLocation opening("opening_" + Ariadne::to_string(progressive));
      DiscreteLocation closing("closing_" + Ariadne::to_string(progressive));
      valve.new_mode(idle);
      valve.new_mode(opening);
      valve.new_mode(closing);

      // 5. Registration of the dynamics for each location

      valve.set_dynamics(idle, valvelevel, 0.0);
      valve.set_dynamics(opening, valvelevel, 1.0/opening_time);
      valve.set_dynamics(closing, valvelevel, -1.0/opening_time);

      /// 6. Transitions

      // Guards
      // The library assumes that given a guard g, the relation g >= 0 must hold in the current mode to have a transition

      RealExpression valvelevel_geq_one = valvelevel - 1.0;
      RealExpression valvelevel_leq_zero = -valvelevel;

      // Resets
      // We need to define a reset for each output variable of the automaton.

      std::map<RealVariable,RealExpression> rst_valvelevel_one;
      rst_valvelevel_one[valvelevel] = 1.0;
      std::map<RealVariable,RealExpression> rst_valvelevel_zero;
      rst_valvelevel_zero[valvelevel] = 0.0;

      // Forced transitions: transitions which implicitly have complementary guards and consequently
      // force the transition to be taken immediately

      valve.new_forced_transition(e_idle, opening, idle, rst_valvelevel_one, valvelevel_geq_one);
      valve.new_forced_transition(e_idle, closing, idle, rst_valvelevel_zero, valvelevel_leq_zero);
      valve.new_unforced_transition(e_open, idle, opening);
      valve.new_unforced_transition(e_close, idle, closing);

      return valve;

    }

  }
