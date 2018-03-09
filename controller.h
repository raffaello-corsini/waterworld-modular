/***************************************************************************
*            controller.h
*
*  These file is used to describe the valve of a watertank.
*
*  Copyright  2018  Raffaello Corsini, Luca Geretti
*
****************************************************************************/

/*
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#ifndef TUTORIAL_SYSTEM_H_
#define TUTORIAL_SYSTEM_H_
*/

// Including this library to work with strings.
// #include <string>

#include <ariadne.h>

namespace Ariadne {

  // Posso passargli un automa? O meglio passargli solo la variabile?
  HybridIOAutomaton getController(
    // Controlled tank's waterlevel.
    RealVariable waterlevel,
    // Parameter hmin, hmax, delta for the analysis.
    RealParameter hmin,
    RealParameter hmax,
    RealParameter delta,
    // Controlled tank's valve
    HybridIOAutomaton valve,
    // This int represents the number of this component.
    int progressive)
    {

      // Conversion of the progressive integer to a String.
      String number = Ariadne::to_string(progressive);

      // 1. Automaton
      HybridIOAutomaton controller("controller" + number);

      // 2. Registration of the input/output variables
      controller.add_input_var(waterlevel);

      // 3. Registration of the events

      // Prendo gli eventi che arrivano dalla valvola.
      set<DiscreteEvent> events = valve.input_events();
      set<DiscreteEvent>::iterator it = events.begin();

      // La seguente è come leggono in hybrid_io_automaton.cc l'evento dall'iteratore.
      // Io provo a riutilizzarlo tale e quale.
      // DiscreteEvent(input_event_it->name()

      DiscreteEvent close_event = DiscreteEvent(it->name());
      controller.add_output_event(close_event);
      it++;
      DiscreteEvent open_event = DiscreteEvent(it->name());
      controller.add_output_event(open_event);

      // 4. Registration of the locations

      DiscreteLocation rising("rising" + number);
      DiscreteLocation falling("falling" + number);
      controller.new_mode(rising);
      controller.new_mode(falling);

      // 5. Transitions

      RealExpression waterlevel_leq_hmax = waterlevel - hmax - delta; // x <= hmax + delta
      RealExpression waterlevel_geq_hmin = hmin - delta - waterlevel; // x >= hmin - delta
      controller.new_invariant(rising, waterlevel_leq_hmax);
      controller.new_invariant(falling, waterlevel_geq_hmin);
      RealExpression waterlevel_geq_hmax = waterlevel - hmax + delta; // x >= hmax - delta
      RealExpression waterlevel_leq_hmin = hmin + delta - waterlevel; // x <= hmin + delta
      controller.new_unforced_transition(close_event, rising, falling, waterlevel_geq_hmax);
      controller.new_unforced_transition(open_event, falling, rising, waterlevel_leq_hmin);

      return controller;

    }

  }
