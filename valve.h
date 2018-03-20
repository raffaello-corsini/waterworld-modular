/***************************************************************************
*            valve.h
*
*  These file is used to describe the valve of a watertank
*  with unforced transitions.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
#ifndef VALVE_H_
#define VALVE_H_
*/

#include <ariadne.h>

namespace Ariadne {

  HybridIOAutomaton getValve(
    // Valve's opening time.
    RealParameter opening_time,
    // Valve's opening level.
    RealVariable valvelevel,
    // This int represents the number of this component.
    int progressive){

      // Conversion of the progressive integer to a String.
      String number = Ariadne::to_string(progressive);

      // 1. Automaton
      HybridIOAutomaton valve("valve" + number);

      // 2. Registration of the input/output variables
      valve.add_output_var(valvelevel);

      // 3 Registration of the input/internal events
      DiscreteEvent e_open("e_open_" + number);
      DiscreteEvent e_close("e_close_" + number);
      DiscreteEvent e_idle("e_idle_" + number);
      valve.add_input_event(e_open);
      valve.add_input_event(e_close);
      valve.add_internal_event(e_idle);

      // 4. Registration of the locations
      DiscreteLocation idle("idle_" + number);
      DiscreteLocation opening("opening_" + number);
      DiscreteLocation closing("closing_" + number);
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
